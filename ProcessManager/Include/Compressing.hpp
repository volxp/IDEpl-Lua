#pragma once
#include <cstdint>
#include <Windows.h>
#include <Luau/Compiler.h>
#include <Luau/BytecodeBuilder.h>
#include <Luau/BytecodeUtils.h>
#include <Luau/Bytecode.h>
#include <Zstd/include/zstd/xxhash.h>
#include <Zstd/include/zstd/zstd.h>
#include <blake3.h>

static constexpr BYTE BYTECODE_SIGNATURE[4] = { 'R', 'S', 'B', '1' };
static constexpr uint8_t BYTECODE_HASH_MULTIPLIER = 41;
static constexpr uint32_t BYTECODE_HASH_SEED = 42u;

static constexpr uint32_t MAGIC_A = 0x4C464F52; // L, F, O, R
static constexpr uint32_t VerificationXor = 0x946AC432;
static constexpr uint8_t  KEY_BYTES[4] = { 0x52, 0x4F, 0x46, 0x4C }; // R, O, F, L 

static inline uint8_t rotl8(uint8_t value, int shift) {
    shift &= 7;
    return (value << shift) | (value >> (8 - shift));
}





class BytecodeEncoderClass : public Luau::BytecodeEncoder {
    inline void encode(uint32_t* Data, size_t Size) override {
        for (auto i = 0u; i < Size;) {
            auto& OpCode = *reinterpret_cast<uint8_t*>(Data + i);
            i += Luau::getOpLength(LuauOpcode(OpCode));
            OpCode *= 227;
        }
    }
};

namespace LuauCompress {
    static std::string ZstdCompress(const std::string_view Bytecode)
    {
        const auto MaxSize = ZSTD_compressBound(Bytecode.size());
        auto Buffer = std::vector<char>(MaxSize + 8);

        memcpy(&Buffer[0], BYTECODE_SIGNATURE, 4);

        const auto Size = static_cast<uint32_t>(Bytecode.size());
        memcpy(&Buffer[4], &Size, sizeof(Size));

        const auto compressed_size = ZSTD_compress(&Buffer[8], MaxSize, Bytecode.data(), Bytecode.size(), ZSTD_maxCLevel());
        if (ZSTD_isError(compressed_size))
            return "";

        const auto FinalSize = compressed_size + 8;
        Buffer.resize(FinalSize);

        const auto HashKey = XXH32(Buffer.data(), FinalSize, BYTECODE_HASH_SEED);
        const auto Bytes = reinterpret_cast<const uint8_t*>(&HashKey);

        for (auto i = 0u; i < FinalSize; ++i)
            Buffer[i] ^= (Bytes[i % 4] + i * BYTECODE_HASH_MULTIPLIER) & 0xFF;

        return std::string(Buffer.data(), FinalSize);
    }

    static std::string Compile(const std::string& source)
    {
        BytecodeEncoderClass Encoder;

        static const char* MutableGlobals[] = {
            ("Game"),
            ("Workspace"),
            ("game"),
            ("plugin"),
            ("script"),
            ("shared"),
            ("workspace"),
            ("_G"),
            nullptr
        };

        static const char* VectorLib = ("Vector3");
        static const char* VectorFunction = ("new");

        Luau::CompileOptions Compilation;
        Compilation.optimizationLevel = 1;
        Compilation.debugLevel = 1;

        Compilation.vectorLib = VectorLib;
        Compilation.vectorCtor = VectorFunction;
        Compilation.vectorType = VectorLib;
        Compilation.mutableGlobals = MutableGlobals;

        return Luau::compile(source, Compilation, { true, true }, &Encoder);
    }


    static std::string BytecodeXor(const std::string& bytecode) {
        if (bytecode.empty()) {
            return "";
        }

        constexpr uint32_t FOOTER_SIZE = 40u;

        std::vector<uint8_t> blake3_hash(32);
        {
            blake3_hasher hasher;
            blake3_hasher_init(&hasher);
            blake3_hasher_update(&hasher, bytecode.data(), bytecode.size());
            blake3_hasher_finalize(&hasher, blake3_hash.data(), blake3_hash.size());
        }

        std::vector<uint8_t> transformed_hash(32);

        for (int i = 0; i < 32; ++i) {
            uint8_t byte = KEY_BYTES[i & 3];
            uint8_t hash_byte = blake3_hash[i];
            uint8_t combined = byte + i;
            uint8_t result;

            switch (i & 3) {
            case 0: {
                int shift = ((combined & 3) + 1);
                result = rotl8(hash_byte ^ ~byte, shift);
                break;
            }
            case 1: {
                int shift = ((combined & 3) + 2);
                result = rotl8(byte ^ ~hash_byte, shift);
                break;
            }
            case 2: {
                int shift = ((combined & 3) + 3);
                result = rotl8(hash_byte ^ ~byte, shift);
                break;
            }
            case 3: {
                int shift = ((combined & 3) + 4);
                result = rotl8(byte ^ ~hash_byte, shift);
                break;
            }
            }
            transformed_hash[i] = result;
        }

        std::vector<uint8_t> footer(FOOTER_SIZE, 0);

        uint32_t first_hash_dword = *reinterpret_cast<uint32_t*>(transformed_hash.data());
        uint32_t footer_prefix = first_hash_dword ^ VerificationXor;
        memcpy(&footer[0], &footer_prefix, 4);

        uint32_t xor_ed = first_hash_dword ^ MAGIC_A;
        memcpy(&footer[4], &xor_ed, 4);

        memcpy(&footer[8], transformed_hash.data(), 32);

        std::string signed_bytecode = bytecode;
        signed_bytecode.append(reinterpret_cast<const char*>(footer.data()), footer.size());

        return ZstdCompress(signed_bytecode);
    }

};
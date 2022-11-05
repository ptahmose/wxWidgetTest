#pragma once

class CompressionOptions
{
public:
    enum class WhatToCompress
    {
        kOnlyUncompressed = 0,
        kUncompressedAndZstd,
        kUncompressedAndZstdAndJpgxr
    };

    WhatToCompress what_to_compress{ WhatToCompress::kOnlyUncompressed };
    int zstd_level{ 0 };
};

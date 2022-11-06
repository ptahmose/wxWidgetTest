#pragma once

/// The options for the compression operation.
class CompressionOptions
{
public:

    /// Values that represent different options which subblocks are to be processed.
    enum class WhatToCompress
    {
        /// An enum constant representing the option "only uncompressed subblocks are being compressed".
        kOnlyUncompressed = 0,

        /// An enum constant representing the option "uncompressed subblock and with zstd-compressed subblocks" are being compressed.
        kUncompressedAndZstd,

        /// An enum constant representing the option "all subblocks which are decodeable by libCZI" get compressed.
        /// This means currently: uncompressed subblocks, zstd- and JPGXR-compressed subblocks.
        kUncompressedAndZstdAndJpgxr    
    };

    WhatToCompress what_to_compress{ WhatToCompress::kOnlyUncompressed };

    int zstd_level{ 0 };
};

<?php

/**
 * @generate-class-entries
 * @generate-c-enums
 */

namespace Encoding
{
    /**
     * @strict-properties
     */
    class EncodingError extends \Error
    {
    }

    /**
     * @strict-properties
     */
    class EncodingException extends \Exception
    {
    }

    /**
     * @strict-properties
     */
    class UnableToDecodeException extends EncodingException
    {
    }

    /**
     * @strict-properties
     */
    class UnableToEncodeException extends EncodingException
    {
    }

    enum Base16
    {
        case Upper;
        case Lower;
    }

    enum Base32
    {
        case Ascii;
        case Hex;
        case Crockford;
        case Z;
    }

    enum Base58
    {
        case Bitcoin;
        case Flickr;
    }

    enum Base64
    {
        case Standard;
        case UrlSafe;
        case Imap;
    }

    enum Base85
    {
        case Adobe;
        case Z85;
        case Git;
    }

    enum PaddingMode
    {
        case VariantControlled;
        case StripPadding;
        case PreservePadding;
    }

    enum DecodingMode
    {
        case Forgiving;
        case Strict;
    }

    enum TimingMode
    {
        case Variable;
        case Constant;
    }

    /**
     * @throws UnableToEncodeException
     */
    function base16_encode(string $data, Base16 $variant = Base16::Upper, TimingMode $timingMode = TimingMode::Variable): string {}

    /**
     * @throws UnableToDecodeException
     */
    function base16_decode(string $data, Base16 $variant = Base16::Upper, DecodingMode $decodingMode = DecodingMode::Strict, TimingMode $timingMode = TimingMode::Variable): string {}

    /**
     * @throws UnableToEncodeException
     */
    function base32_encode(string $data, Base32 $variant = Base32::Ascii, PaddingMode $paddingMode = PaddingMode::VariantControlled, TimingMode $timingMode = TimingMode::Variable): string {}

    /**
     * @throws UnableToDecodeException
     */
    function base32_decode(string $data, Base32 $variant = Base32::Ascii, DecodingMode $decodingMode = DecodingMode::Strict, TimingMode $timingMode = TimingMode::Variable): string {}

    /**
     * @throws UnableToEncodeException
     */
    function base58_encode(string $data, Base58 $variant = Base58::Bitcoin, TimingMode $timingMode = TimingMode::Variable): string {}

    /**
     * @throws UnableToDecodeException
     */
    function base58_decode(string $data, Base58 $variant = Base58::Bitcoin, TimingMode $timingMode = TimingMode::Variable): string {}

    /**
     * @throws UnableToEncodeException
     */
    function base64_encode(string $data, Base64 $variant = Base64::Standard, PaddingMode $paddingMode = PaddingMode::VariantControlled, TimingMode $timingMode = TimingMode::Variable): string {}

    /**
     * @throws UnableToDecodeException
     */
    function base64_decode(string $data, Base64 $variant = Base64::Standard, DecodingMode $decodingMode = DecodingMode::Strict, TimingMode $timingMode = TimingMode::Variable): string {}

    /**
     * @throws UnableToEncodeException
     */
    function base85_encode(string $data, Base85 $variant, PaddingMode $paddingMode = PaddingMode::VariantControlled, TimingMode $timingMode = TimingMode::Variable): string {}

    /**
     * @throws UnableToDecodeException
     */
     function base85_decode(string $data, Base85 $variant, DecodingMode $decodingMode = DecodingMode::Strict, TimingMode $timingMode = TimingMode::Variable): string {}
}

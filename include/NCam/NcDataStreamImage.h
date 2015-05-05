/*
*  Copyright (c) 2012-2014 Ncam Technologies Ltd. All rights reserved.
*  Unpublished - rights reserved under the copyright laws of the
*  United States. Use of a copyright notice is precautionary only
*  and does not imply publication or disclosure.
*  This software contains confidential information and trade secrets
*  of Ncam Technologies Limited. Use, disclosure, or reproduction
*  is prohibited without the prior express written permission of
*  Ncam Technologies Limited.
*/
#ifndef NCDATASTREAMIMAGE_H
#define NCDATASTREAMIMAGE_H
#include <NcDataStreamBase.h>
/** \addtogroup SDK
 *  @{
 */
/**
 * @brief The NcDataStreamImage class is a container of image transfer from server to client.
 */
class NC_SHARED_LIB NcDataStreamImage: public NcDataStreamBase
{
public:
    /**
     * @brief Helper for the number of channels.
     */
    enum Channels
    {
        UnknownChannels     = 0, ///< Un-specified number of channels.
        Nc1Channel          = 1, ///< Single channel image.
        Nc2Channels         = 2, ///< Two channels image (eg. used for distortion).
        NcRGB               = 3, ///< Three channels imge, in Red Green Blue.
        NcRGBD              = 4  ///< Four channels image, in Red Green Blue Alpha.
    };

    /**
     * @brief Helper for the depth of image.
     */
    enum Depth
    {
        UnknownDepth        = 0, ///< Un-specified depth of image.
        OneBytePerPixel     = 1, ///< Single byte per channel.
        OneFloatPerPixel    = 4  ///< Four bytes per channels (size of a float).
    };

    /**
     * @brief Header of a sent/received image.
     */
    struct NcTrackStreamImageHeader
    {
        PacketType_t mPacketType; ///< Type of image received.
        uint16_t mChannels;       ///< Number of channels.
        uint16_t mDepth;          ///< Depth of each channel.
        uint32_t mWidth;          ///< Width of the image.
        uint32_t mHeight;         ///< Height of the image.
        TimeCode_t mTimeCode;     ///< Timecode associated to the image.
    };

// Methods
public:
    /**
     * @brief Release the container.
     */
    virtual ~NcDataStreamImage();
    /**
     * @brief
     * @return The size of the containers in bytes.
     */
    virtual size_t GetSizeInBytes() const;
    /**
     * @brief
     * @return The packet type.
     */
    ePacketType GetPacketType() const;
    virtual bool FromPtr(const uint8_t *lptr, const size_t& lMaxSize);

    /**
     * @brief Set the specified timecode for the image.
     * @param lTimeCode The time code to set.
     */
    void SetTimeCode(const uint32_t& lTimeCode);
    /**
     * @brief Set the size of the container (create the header).
     * @param[in] lWidth Width of the image.
     * @param[in] lHeight Height of the image.
     * @return True if the header as been successfully created.
     */
    bool SetSize(const uint32_t& lWidth, const uint32_t& lHeight);
    /**
     * @brief Returns the size of the container.
     * @param[out] lWidth Width of the container header allocated.
     * @param[out] lHeight Height of the container header created.
     */
    void GetSize(uint32_t& lWidth, uint32_t& lHeight) const;
    /**
     * @brief
     * @return The Width of the allocated container header.
     */
    uint32_t GetWidth() const;
    /**
     * @brief
     * @return The height of the allocated container header.
     */
    uint32_t GetHeight() const;
    /**
     * @brief
     * @return The number of channels of the allocated container header.
     */
    Channels GetChannels()const;
    /**
     * @brief
     * @return The depth of the allocated container header.
     */
    Depth GetDepth() const;
    /**
     * @brief Converts the received image into a uint8_t pointer (for compatibility with sending or usual image API).
     * @return A constant raw data pointer to image pixels.
     */
    const uint8_t* GetImagePtr() const;
    /**
     * @brief Converts the received image into a uint8_t pointer (for compatibility with sending or usual image API).
     * @return An access to the raw data pointer to image pixels.
     */
    uint8_t* GetImagePtr();
    /**
     * @brief
     * @return The size in byte of the container.
     */
    size_t GetImageSizeInBytes() const;

protected:
    /**
     * @brief Protected constructor to the image.
     * @param lChannels Number of channels
     * @param lDepth Depth of each channel.
     */
    NcDataStreamImage(Channels lChannels = UnknownChannels,Depth lDepth = UnknownDepth);
    virtual uint8_t* Ptr();
    virtual const uint8_t* Ptr()const ;

    uint8_t* mData; ///< Raw data pointer, where the first Bytes of mData are containing one NcTrackStreamImageHeader
    size_t mPreviousImageSizeInBytes; ///< Pointer to the previous image.
    /**
     * @brief
     * @return The header size in bytes (for transfer purpose usually).
     */
    size_t GetHeaderInBytes() const;
    /**
     * @brief Update current image size if bigger than previously received image (eg. for resolution change).
     */
    void UpdateImageSize();

private:
    NcDataStreamImage& operator =(NcDataStreamImage&){ return *this;}
    NcDataStreamImage(NcDataStreamImage& ){}
};
/** @}*/
#endif // NCDATASTREAMIMAGE_H

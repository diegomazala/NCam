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
#ifndef NCDATASTREAMBASE_H
#define NCDATASTREAMBASE_H
/** \addtogroup SDK
 *  @{
 */
#include <LibNcDataStreaming_global.h>

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#ifdef WIN32
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

/**
 * @brief The NcDataStreamBase class is a virtual class use to get access to all type of packets transfer through the Ncam Data Streaming library.
 */
class NC_SHARED_LIB NcDataStreamBase
{
public:
    typedef uint32_t PacketType_t;
    typedef uint32_t TimeCode_t;

    static const size_t PacketType_s;
    /**
     * @brief Type of packet transfer through the Ncam Data Streaming library.
     */
    enum ePacketType
    {
        UnknownType         = 0x0000, ///< Unknow packet type.
        CameraTracking      = 0x0001, ///< Camera tracking packet type.
        DepthImage          = 0x0002, ///< Depth image packet type.
        FilmImage           = 0x0004, ///< Film camera packet type.
        CompositeImage      = 0x0008, ///< Composite image made by Ncam Reality packet type.
        DistortMap          = 0x0010, ///< Distortion map (to apply to the CG to match the film camera image).
        OpticalParameters   = 0x0020, ///< Optical parameter packet type (reserve for futher usage).
        Capabilities        = 0x0040, ///< Capabilities of the Ncam reality server launch.
        Query               = 0x0080, ///< Token send to the server to query capabilities or format.
        TypeMask            = CameraTracking|DepthImage|FilmImage|CompositeImage|DistortMap|OpticalParameters|Capabilities|Query ///< Mask of values to get the packet sent or received.
    };

    // Methods
public:
    /**
     * @brief
     * @return The pointer to the raw data values.
     */
    uint8_t *ToPtr();
    /**
     * @brief
     * @return The constant access to the raw data pointed.
     */
    const uint8_t *ToPtr() const;
    /**
     * @brief Copy data pointed by lptr, of size lSize, to the internal format.
     * @param lptr The data to store.
     * @param lSize The size of lptr to store.
     * @return True if internal size is large enough, false otherwise, and call GetSizeInBytes error callback.
     */
    virtual bool FromPtr(const uint8_t* lptr, const size_t& lSize);
    /**
     * @brief Copy data pointed py lptr, of static size lSize to the internal format.
     * @param lptr The static data to store (through a template syntax).
     * @return True if internal size is large enough, false otherwise.
     * @see FromPtr for the dynamic approach.
     */
    template <size_t lSize>
    bool FromPtr(const uint8_t (&lptr)[lSize]);

    /**
     * @brief
     * @return The size in bytes of the internal storage.
     */
    virtual size_t GetSizeInBytes() const = 0;
    /**
     * @brief
     * @return The packet type.
     * @see ePacketType for type of packet available.
     */
    virtual ePacketType GetPacketType() const = 0;
    /**
     * @brief Virtual destructor (does nothing).
     */
    virtual ~NcDataStreamBase();

protected:

    /**
     * @brief Callback on packet error during reading/copy.
     * @param lErrorDecription the error message set by the calling method.
     */
    virtual void OnDecodingFromPtrError(const std::string& lErrorDecription = std::string());

    /**
     * @brief Pure virtual function used to convert any data type to the internal format.
     * @return Return a uint8_t pointer to data to transfer.
     */
    virtual uint8_t* Ptr() = 0;
    /**
     * @brief Pure virtual function to get access to a constant pointer of type uint8_t.
     * @return A constant pointer of type uint8_t to data to transfer.
     */
    virtual const uint8_t* Ptr() const = 0;
    /**
     * @brief Protected constructor (does nothing).
     */
    NcDataStreamBase();
};

typedef std::map<NcDataStreamBase::PacketType_t,NcDataStreamBase*> Packets_t;
/** @}*/
#endif // NCDATASTREAMBASE_H

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
/** @internal */
#ifndef NCDATASTREAMSERVERBASE_H
#define NCDATASTREAMSERVERBASE_H
#include <NcDataStreaming.h>
#include <NcDataStreamIOBase.h>
#include <map>
class NC_SHARED_LIB NcDataStreamServerBase: public NcDataStreamIOBase
{
public:
    explicit NcDataStreamServerBase(const NcDataStreamBase::PacketType_t& lAvailablePackets,const NcDataStreamBase::PacketType_t& lEnabledPackets);
    virtual ~NcDataStreamServerBase();

    void SetCapabilities(NcDataStreamCapabilities& lCapabilities);
    const NcDataStreamCapabilities& GetCapabilities() const;
    void SetConnectionParameters(const IpPort_t& lIpPort);

    bool StartStreaming();
    bool StopStreaming();
    bool DoStreaming();

protected:

    //Error handling
    virtual void OnStartStreamingError(const std::string& lErrorDecription = std::string());
    virtual void OnStopStreamingError(const std::string& lErrorDecription = std::string());
    virtual void OnDoStreamingError(const std::string& lErrorDecription = std::string());

    virtual Packets_t& MapPackets(bool& lSuccess) = 0;
    virtual void UnmapPackets() = 0;
    virtual bool WaitForNewDataReady(uint64_t lTimeOutInMs) = 0;

private:

    bool GetQueryAndReturnCapabilities();
    bool SendActivatedData();
    bool SendSearchedPacket(const Packets_t& lPackets, NcDataStreamBase::PacketType_t lSearchedPacket);
    const NcDataStreamBase* GetPacket(const Packets_t& lPackets, NcDataStreamBase::PacketType_t lSearchedPacket);
    NcDataStreamCapabilities mServerCapabilities;
};

#endif // NCDATASTREAMSERVERBASE_H

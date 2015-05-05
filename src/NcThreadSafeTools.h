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

#include <QMutex>
#include <QWaitCondition>
#include <QThread>
#include <QReadWriteLock>
/** \addtogroup MyGUIClientTvGlobo
 *  @{
 */


template <class ProtectedResource, QReadWriteLock::RecursionMode lMode = QReadWriteLock::Recursive>
class NcThreadSafeResource
{
public:
    inline NcThreadSafeResource():
        mProtectedResource(),
        mQReadWriteLock(lMode)
    {

    }

    inline NcThreadSafeResource(const ProtectedResource& lProtectedResource):
        mProtectedResource(lProtectedResource),
        mQReadWriteLock(lMode)
    {

    }

private:
    inline NcThreadSafeResource(const NcThreadSafeResource& lsrc):
        mProtectedResource(lsrc.mProtectedResource),
        mQReadWriteLock(lMode)
    {

    }

    const NcThreadSafeResource& operator=(const NcThreadSafeResource& lsrc)
    {
        if (&lsrc == this)
            return *this;
        mProtectedResource = lsrc.mProtectedResource;
    }

public:
    virtual ~NcThreadSafeResource()
    {

    }

    inline ProtectedResource& TryMapRO(bool& lSucess)
    {
        lSucess = mQReadWriteLock.tryLockForRead();
        return mProtectedResource;
    }

    inline ProtectedResource& TryMapRW(bool& lSucess)
    {
        lSucess = mQReadWriteLock.tryLockForWrite();
        return mProtectedResource;
    }

    inline ProtectedResource& TryMapRW()
    {
        mQReadWriteLock.lockForWrite();
        return mProtectedResource;
    }

    inline ProtectedResource& MapRO()
    {
        mQReadWriteLock.lockForRead();
        return mProtectedResource;
    }

    inline ProtectedResource& MapRW()
    {
        mQReadWriteLock.lockForWrite();
        return mProtectedResource;
    }

    inline void UnMap()
    {
        mQReadWriteLock.unlock();
    }

    inline ProtectedResource GetValue()
    {
        mQReadWriteLock.lockForRead();
        ProtectedResource lRet = mProtectedResource;
        mQReadWriteLock.unlock();
        return lRet;
    }

    inline void SetValue(const ProtectedResource &lOrig)
    {
        mQReadWriteLock.lockForWrite();
        mProtectedResource = lOrig;
        mQReadWriteLock.unlock();
    }

    inline QReadWriteLock& GetLocker()
    {
        return mQReadWriteLock;
    }

private:
    ProtectedResource mProtectedResource;
    QReadWriteLock mQReadWriteLock;
};

template <class ProtectedResource,QReadWriteLock::RecursionMode lMode = QReadWriteLock::Recursive>
class NcResourceReader
{
public :

    inline NcResourceReader(NcThreadSafeResource<ProtectedResource,lMode>& src):
        mResourceProtector(src),
        mResource(src.MapRO())
    {
    }

    virtual inline ~NcResourceReader()
    {
        mResourceProtector.UnMap();
    }

    inline ProtectedResource& Get()
    {
        return mResource;
    }

    inline QReadWriteLock* GetLocker()
    {
        return &mResourceProtector.GetLocker();
    }

protected:
    NcThreadSafeResource<ProtectedResource,lMode>& mResourceProtector;
    ProtectedResource& mResource;
};


template <class ProtectedResource,QReadWriteLock::RecursionMode lMode = QReadWriteLock::Recursive>
class NcResourceWriter
{
public :

    inline NcResourceWriter(NcThreadSafeResource<ProtectedResource,lMode>& src):
        mResourceProtector(src),
        mResource(src.MapRW())
    {
    }

    virtual inline ~NcResourceWriter()
    {
        mResourceProtector.UnMap();
    }

    inline ProtectedResource& Get()
    {
        return mResource;
    }
    inline QReadWriteLock* GetLocker()
    {
        return &mResourceProtector.GetLocker();
    }

protected:
    NcThreadSafeResource<ProtectedResource,lMode>& mResourceProtector;
    ProtectedResource& mResource;
};


/// \class NcDataBufferSwap
/// \brief Base Class for a safe Way to exchange Data between Threads.
template<class T, int NbBuffers = 3>
class NcDataBufferSwap
{
public:

    /// \brief Default constructor
    NcDataBufferSwap():
        mUsedRessourceBufferId(-1),
        mProductedBufferId(-1),
        mProductorCurrentBufferId(0),
        mDataArray()
    {

    }

    /// \brief Default Destructor
    virtual ~NcDataBufferSwap()
    {

    }

    bool WaitForNewProductAvailable(unsigned long luiTime = ULONG_MAX)
    {
        NcResourceReader<int,QReadWriteLock::NonRecursive> lReader(mProductedBufferId);
        if (mUsedRessourceBufferId.GetValue() == lReader.Get())
            if (false == mSomethingProduced.wait(lReader.GetLocker(),luiTime))
            {
                return false;
            }
        return true;
    }

    /// \brief Return true if there is something new for the comsumer.
    bool IsNewProductAvailable()
    {
        bool IsASuccess = false;
        int lValue1 = mProductedBufferId.TryMapRO(IsASuccess);
        if (IsASuccess)
        {
            int lValue2 = mUsedRessourceBufferId.TryMapRO(IsASuccess);
            if (IsASuccess)
            {
                IsASuccess = (lValue1 != lValue2);
                mUsedRessourceBufferId.UnMap();
            }
            mProductedBufferId.UnMap();
        }
        return IsASuccess;
    }

    T GetLastBuffer(bool* lIsASuccess)
    {
        NcResourceReader<int,QReadWriteLock::NonRecursive> lReader(mProductedBufferId);
        if (lReader.Get() < 0)
        {
            *lIsASuccess = false;
            return mDataArray[0];
        }
        *lIsASuccess = true;
        return mDataArray[lReader.Get()];
    }

    T& MapConsumerBuffer(bool& lIsASuccess, bool lWaitForNewData, unsigned long luiTime = ULONG_MAX)
    {
        lIsASuccess = false;
        NcResourceReader<int,QReadWriteLock::NonRecursive> lReader(mProductedBufferId);
        if (lWaitForNewData)
        {
            if (mUsedRessourceBufferId.GetValue() == lReader.Get())
                if (false == mSomethingProduced.wait(lReader.GetLocker(),luiTime))
                    return (mDataArray[0]);
        }
        else
        {
            if (lReader.Get() < 0)
                return mDataArray[0];
        }
        lIsASuccess = true;
        mUsedRessourceBufferId.SetValue(lReader.Get());
        return mDataArray[lReader.Get()];
    }

    void UnMapConsumerBuffer()
    {
        NcResourceReader<int,QReadWriteLock::NonRecursive> lReader(mProductedBufferId);
    }

    /// \brief Map a Productor buffer
    T& MapProductorBuffer()
    {
        return mDataArray[mProductorCurrentBufferId.GetValue()];
    }

    /// \brief Release the last Mapped Productor buffer
    void UnMapProductorBuffer()
    {
        int& lProducedBufferId = mProductedBufferId.MapRW();
        int& lProductorCurrentBufferId = mProductorCurrentBufferId.MapRW();
        lProducedBufferId = lProductorCurrentBufferId;

        while(lProductorCurrentBufferId == lProducedBufferId || lProductorCurrentBufferId == mUsedRessourceBufferId.GetValue())
            lProductorCurrentBufferId = (lProductorCurrentBufferId + 1 )%NbBuffers;

        mProductorCurrentBufferId.UnMap();
        mProductedBufferId.UnMap();

        mSomethingProduced.wakeOne();
    }

protected:
    NcThreadSafeResource <int, QReadWriteLock::NonRecursive>    mUsedRessourceBufferId;
    NcThreadSafeResource <int, QReadWriteLock::NonRecursive>    mProductedBufferId;
    NcThreadSafeResource <int, QReadWriteLock::NonRecursive>    mProductorCurrentBufferId;
    QWaitCondition mSomethingProduced;
    T mDataArray[NbBuffers];
};
/** @}*/

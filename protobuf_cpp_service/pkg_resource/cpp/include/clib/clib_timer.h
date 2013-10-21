/**
 * @file    clib_timer.h
 * @version 1.0
 * @author  rogeryang
 * @date    2012/12/17
 * @brief   ��ʱ����������/����/��
 */
#ifndef _CLIB_TIMER_H_
#define _CLIB_TIMER_H_
#include <map>
#include <string>
#include <sys/time.h>

/** @brief ������ʱ�Ĺ�����
 *         ��start()����һ�������㣬 end()ֹͣһ�������㣬
 *         get_ms/get_us��ȡstart��end��ĺ�ʱ
 *         ���ַ�����ʶÿ��������
 *
 */
class clib_timer
{
public:
    clib_timer();
    ~clib_timer();

    /** @brief ����һ��������
     *
     *  @param[in] as_name �����������
     *  @return ��
     */
    void start( const char* as_name );

    /** @brief ����һ��������
     *
     *  @param[in] as_name �����������
     *  @return ��
     */
    void end( const char* as_name );

    /** @brief ��ȡһ��������ĺ�ʱ(����)
     *
     *  @param[in] as_name �����������
     *  @return ������ĺ�ʱ, ��������㲻����, ����0
     */
    uint32_t get_ms( const char* as_name );

    /** @brief ��ȡһ��������ĺ�ʱ(΢��)
     *
     *  @param[in] as_name �����������
     *  @return ������ĺ�ʱ, ��������㲻����, ����0
     */
    uint32_t get_us( const char* as_name );

    /** @brief ��������ȡһ��������ĺ�ʱ(����)
     *
     *  @param[in] as_name �����������
     *  @return ������ĺ�ʱ, ��������㲻����, ����0
     */
    uint32_t end_get_ms( const char* as_name );

    /** @brief ��������ȡһ��������ĺ�ʱ(΢��)
     *
     *  @param[in] as_name �����������
     *  @return ������ĺ�ʱ, ��������㲻����, ����0
     */
    uint32_t end_get_us( const char* as_name );

    /* ��������Ϣ��map�����Ͷ��� */
    typedef struct _tagTimePointInfo
    {
        struct timeval tvStart;
        struct timeval tvEnd;
        uint32_t uiUseTimeMS;
        uint32_t uiUseTimeUS;
        _tagTimePointInfo():uiUseTimeMS(0), uiUseTimeUS(0)
        {
            memset(&tvStart, 0, sizeof(tvStart) );
            memset(&tvEnd, 0, sizeof(tvEnd) );
        }
    }TimePointInfo;

    typedef std::map<std::string, TimePointInfo> TimerResultMap;
    typedef TimerResultMap::iterator TimerResultMapIter;
    typedef TimerResultMap::const_iterator TimerResultMapConstIter;

    /** @brief ��ȡ���в����������
     *
     *  @return ���в������map, keyΪ����������, valueΪ��������Ϣ(TimePointInfo)
     */
    TimerResultMap get_all_timepoint();

private:

    /** @brief ���ݲ�������Ϣ�еĿ�ʼ����ʱ������ʱ���ڲ�ʹ��
     *
     */
    static void cal_result( TimePointInfo& info )
    {
        info.uiUseTimeUS = ( info.tvEnd.tv_sec - info.tvStart.tv_sec)*1000000+info.tvEnd.tv_usec-info.tvStart.tv_usec;
        info.uiUseTimeMS = info.uiUseTimeUS/1000;
    }

    /* ��Ų�������Ϣ */
    TimerResultMap m_mapResult;
};

/** @brief ��������/���ʽ��ִ��ʱ��
 *         ��Ҫ����clib_timer����Ͳ���������
 *
 * @param[in]    func   ��Ҫ����ִ��ʱ��ĺ���/���ʽ
 * @param[in]    timer  ��������ʱ���clib_timer�����
 * @param[in]    timer_name �����������
 * @code
 *      //����func_test(params)��ִ��ʱ��
 *      clib_timer timer;
 *      int iRet = 0;
 *      char szParam[1024] = {0};
 *
 *      TIME_PROFILE_CALL( iRet=func_test(szParam), timer, "func_test" );
 *
 *      printf( "func_test ret:%d usetime:%u ms\n", iRet, timer.get_ms("func_test") );
 * @endcode
 */
#define TIME_PROFILE_CALL(func, timer, time_point_name ) \
    do\
    {\
        timer.start(time_point_name);\
        func;\
        timer.end(time_point_name);\
    }while(0)

/** @brief ��������/���ʽ��ִ��ʱ��(����)
 *         ���贫��clib_timer����Ͳ���������
 *
 * @param[in]    func   ��Ҫ����ִ��ʱ��ĺ���/���ʽ
 * @param[out]   use_time_ms  �����������ĵ�ʱ��(����)
 * @code
 *      //����func_test(params)��ִ��ʱ��
 *      int iRet = 0;
 *      uint32_t uiUseTimeMs = 0;
 *      char szParam[1024] = {0};
 *
 *      TIME_PROFILE_CALL_MS( iRet=func_test(szParam), uiUseTimeMs );
 *
 *      printf( "func_test ret:%d usetime:%u ms\n", iRet, uiUseTimeMs );
 * @endcode
 */
#define TIME_PROFILE_CALL_MS(func, use_time_ms) \
    do\
    {\
        clib_timer timer;\
        static const char* as_tmp_name = "time_profile_call_ms";\
        timer.start(as_tmp_name);\
        func;\
        use_time_ms = timer.end_get_ms(as_tmp_name);\
    }while(0)

/** @brief ��������/���ʽ��ִ��ʱ��(΢��)
 *         ���贫��clib_timer����Ͳ���������
 *
 * @param[in]    func   ��Ҫ����ִ��ʱ��ĺ���/���ʽ
 * @param[out]   use_time_us  �����������ĵ�ʱ��(΢��)
 * @code
 *      //����func_test(params)��ִ��ʱ��
 *      int iRet = 0;
 *      uint32_t uiUseTimeUs = 0;
 *      char szParam[1024] = {0};
 *
 *      TIME_PROFILE_CALL_US( iRet=func_test(szParam), uiUseTimeUs );
 *
 *      printf( "func_test ret:%d usetime:%u us\n", iRet, uiUseTimeUs );
 * @endcode
 */
#define TIME_PROFILE_CALL_US(func, use_time_us) \
    do\
    {\
        clib_timer timer;\
        static const char* as_tmp_name = "time_profile_call_us";\
        timer.start(as_tmp_name);\
        func;\
        use_time_us = timer.end_get_us(as_tmp_name);\
    }while(0)

inline
clib_timer::clib_timer()
{
}

inline
clib_timer::~clib_timer()
{
}

inline void
clib_timer::start( const char* as_name )
{
    TimePointInfo stTimerInfo;
    gettimeofday( &(stTimerInfo.tvStart), NULL );
    m_mapResult[as_name] = stTimerInfo;
}

inline void
clib_timer::end( const char* as_name )
{
    TimerResultMapIter iter = m_mapResult.find( as_name );
    if( iter != m_mapResult.end() )
    {
        gettimeofday( &(iter->second.tvEnd), NULL );
        cal_result( iter->second );
    }
}

inline uint32_t
clib_timer::get_ms( const char* as_name )
{
    TimerResultMapIter iter = m_mapResult.find( as_name );
    if( iter != m_mapResult.end() )
    {
        return iter->second.uiUseTimeMS;
    }
    else
    {
        return 0;
    }
}

inline uint32_t
clib_timer::get_us( const char* as_name )
{
    TimerResultMapIter iter = m_mapResult.find( as_name );
    if( iter != m_mapResult.end() )
    {
        return iter->second.uiUseTimeUS;
    }
    else
    {
        return 0;
    }
}

inline uint32_t
clib_timer::end_get_ms( const char* as_name )
{
    end( as_name );
    return get_ms( as_name );
}

inline uint32_t
clib_timer::end_get_us( const char* as_name )
{
    end( as_name );
    return get_us( as_name );
}

inline clib_timer::TimerResultMap
clib_timer::get_all_timepoint()
{
    return m_mapResult;
}

#endif


#ifndef __MEMORY_POOL_H__
#define __MEMORY_POOL_H__

#include <iostream>
#include <stdint.h>

const int MEMORY_UNIT_SIZE = 8 * 1024 * 1024;


/*************************************************************************************************
 * 类名：CMemoryPool
 * 功能：内存池基本单元模块，用于业务统计数据的缓存
 * ***********************************************************************************************/
class CMemoryPool
{
public:
	CMemoryPool():m_iFreeSize(0), m_iCapacity(0),m_pszHead(NULL),m_pszFree(NULL),m_bFree(false)
	{
	}
	CMemoryPool(int iSize):m_iFreeSize(0), m_iCapacity(0),m_pszHead(NULL),m_pszFree(NULL),m_bFree(false)
	{
		SetMemoryPoolSize(iSize);	
	}

	~CMemoryPool()
	{
		DestroyMemoryPool();	
	}

	/******************************************************************************************
	 * 函数：Free
	 * 功能：判断该内存池单元是否仍有空闲空间可用
	 * 参数：无
	 * 返回值：true 表示仍有空间可用，false表示该内存池单元空间已满
	 * ****************************************************************************************/
	bool Free()const 
	{ 
		return m_bFree; 
	}
	
	/*****************************************************************************************
	 * 函数：SetFree
	 * 功能：设置内存池单元的可用状态
	 * 参数： 
	 *		输入参数 bFree : false 不做操作，true 表示重置该内存池单元为空
	 * 返回值：无
	 *******************************************************************************************/
	void SetFree(bool bFree) 
	{
		if(m_bFree == true)
		{
			m_iFreeSize = m_iCapacity;
			m_pszFree = m_pszHead;
		}
		m_bFree = bFree; 
	}
	
	/*****************************************************************************************
	 * 函数：AddElement
	 * 功能：往内存池中写入大小为iLen的数据块pszElement
	 * 参数：
	 *		输入参数 pszElement : 表示待写入的数据指针
	 *		         iLen : 表示待写入的数据长度
	 * 返回值：返回数据写入后在内存池单元中的起始指针，若该内存池单元空间不够，则返回NULL
	 * ****************************************************************************************/
	const char * AddElement(const char  *pszElement, int iLen)
	{
		if(iLen < m_iFreeSize && pszElement != NULL)
		{
			const char *pszElementHead = m_pszFree;
			memcpy((void *)pszElementHead, (const void *)pszElement, iLen);
			m_pszFree += iLen;
			*m_pszFree = '\0';
			m_pszFree += 1;
			m_iFreeSize -= (iLen + 1);
			return pszElementHead;
		}
		else
			return NULL;
	}
	
	/*****************************************************************************************
	 * 函数：DestroyMemory
	 * 功能：清除该内存池单元的内存空间
	 * 参数： 无
	 * 返回值：无
	 * ***************************************************************************************/
	void DestroyMemoryPool()
	{
		if(m_pszHead != NULL)
		{
			delete [] m_pszHead;	
			m_pszHead = NULL;
		}
	}

	/****************************************************************************************
	 * 函数：SetMemoryPoolSize
	 * 功能：初始化设置内存池单元大小，并开辟内存空间
	 * 参数：
	 *		输入参数 iSize : 内存池单元大小
	 * 返回值：true表示设置成功，false表示设置失败
	 * *************************************************************************************/
	bool SetMemoryPoolSize(int iSize)
	{
		try
		{
			if(m_pszHead == NULL)
			{
				m_bFree = true;
				m_iCapacity = iSize;
				m_iFreeSize = iSize;
				m_pszHead = new char[iSize];
				m_pszFree = m_pszHead;
				return true;
			}
			else
				return false;
		}
		catch(...)
		{
			return false;	
		}
	}
	
private:
	int m_iFreeSize;
	int m_iCapacity;
	char *m_pszHead;
	char *m_pszFree;
	bool m_bFree;
};

#endif


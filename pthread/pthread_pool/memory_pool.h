#ifndef __MEMORY_POOL_H__
#define __MEMORY_POOL_H__

#include <iostream>
#include <stdint.h>

const int MEMORY_UNIT_SIZE = 8 * 1024 * 1024;


/*************************************************************************************************
 * ������CMemoryPool
 * ���ܣ��ڴ�ػ�����Ԫģ�飬����ҵ��ͳ�����ݵĻ���
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
	 * ������Free
	 * ���ܣ��жϸ��ڴ�ص�Ԫ�Ƿ����п��пռ����
	 * ��������
	 * ����ֵ��true ��ʾ���пռ���ã�false��ʾ���ڴ�ص�Ԫ�ռ�����
	 * ****************************************************************************************/
	bool Free()const 
	{ 
		return m_bFree; 
	}
	
	/*****************************************************************************************
	 * ������SetFree
	 * ���ܣ������ڴ�ص�Ԫ�Ŀ���״̬
	 * ������ 
	 *		������� bFree : false ����������true ��ʾ���ø��ڴ�ص�ԪΪ��
	 * ����ֵ����
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
	 * ������AddElement
	 * ���ܣ����ڴ����д���СΪiLen�����ݿ�pszElement
	 * ������
	 *		������� pszElement : ��ʾ��д�������ָ��
	 *		         iLen : ��ʾ��д������ݳ���
	 * ����ֵ����������д������ڴ�ص�Ԫ�е���ʼָ�룬�����ڴ�ص�Ԫ�ռ䲻�����򷵻�NULL
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
	 * ������DestroyMemory
	 * ���ܣ�������ڴ�ص�Ԫ���ڴ�ռ�
	 * ������ ��
	 * ����ֵ����
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
	 * ������SetMemoryPoolSize
	 * ���ܣ���ʼ�������ڴ�ص�Ԫ��С���������ڴ�ռ�
	 * ������
	 *		������� iSize : �ڴ�ص�Ԫ��С
	 * ����ֵ��true��ʾ���óɹ���false��ʾ����ʧ��
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


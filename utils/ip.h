/*================================================================
*  
*  文件名称：ip.h
*  创 建 者: mongia
*  创建日期：2021年03月08日
*  
================================================================*/

#include <stdio.h>

bool ipvalid(const char* ip)
{
	if(ip == NULL)
		return false;
	
	int a[4];
	if(sscanf(ip,"%d.%d.%d.%d",&a[0],&a[1],&a[2],&a[3]) != 4)  
	{  
		return false;  
	}  

	for(int i = 0; i < 4; ++i)  
	{  
		if(a[i] < 0 || a[i] > 255)  
		{  
			return false;  
		}  
	}  
	return true;  
}

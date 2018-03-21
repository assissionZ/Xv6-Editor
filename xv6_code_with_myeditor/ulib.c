#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"

int
puts(char *s)
{
  return write(1, s, strlen(s));
}

char*
strcpy(char *s, char *t)
{
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
    ;
  return os;
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
    p++, q++;
  return (int) ((unsigned char) *p - (unsigned char) *q);
}

unsigned int
strlen(char *s)
{
  int n = 0;
  for(n = 0; s[n]; n++)
    ;
  return n;
}

//将已开辟内存空间 dst 的首 n 个字节的值设为值 c
void*
memset(void *dst, int c, unsigned int n)
{
  char *d = (char*) dst;

  while(n-- > 0)
    *d++ = c;

  return dst;
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
    if(*s == c)
      return (char*) s;
  return 0;
}

char*
gets(char *buf, int max)
{
  int i = 0, cc;
  char c;

  while(i+1 < max){
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    if(c == '\n' || c == '\r')
      break;
    buf[i++] = c;
  }
  buf[i] = '\0';
  return buf;
}

int
stat(char *n, struct stat *st)
{
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
  r = fstat(fd, st);
  close(fd);
  return r;
}

//ooo
int 
atoi(char* src)
{
	int number = 0; 
	int i=0;
	int pos = strlen(src);
	for(;i<pos;i++)
	{
		if(src[i]==' ') break;
		if(src[i]>57||src[i]<48) return -1;
			number=10*number+(src[i]-48);
	}
	return number;
}

//ooo
//拼接src的前n个字符到dest
char* 
strcat(char* dest, char* src, int n)
{
	if (n <= 0)
		return dest;
	int pos = strlen(dest);
	if (n + pos >= MAX_LINE_LENGTH)
		return dest;
	int i = 0;
	for (; i < n; i++)
		dest[i+pos] = src[i];
	dest[n+pos] = '\0';
	return dest;
}
/*
//ooo
int 
atoi (char s[])
{
	int i,n,sign;
	for(i=0;i>=0;i++)
	{
		if(s[i]==' '){
			
		}
		else{
			break;
		}
	}//跳过空白符
	sign=(s[i]=='-')?-1:1;//判断正负 
	if(s[i]=='+'||s[i]==' -')//跳过符号
	  i++;
	for(n=0;i>=0;i++)
	{
		if(s[i]>='0'&&s[i]<=9)
		{
			n=10*n+(s[i]-'0');//将数字字符转换成整形数字
		}
		else
		{
			break;
		}
	}
	return sign*n;
}
*/

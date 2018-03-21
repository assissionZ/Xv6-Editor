#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "fs.h"
#include "user.h"

int get_line_number(char *text[]);
void show_text(char *text[]);
void text_ins(char *text[], int n);
void text_mod(char *text[], int n);
void text_del(char *text[], int n);
void text_help(char *text[]);
void text_save(char *text[], char *path);
void text_exit(char *text[], char *path);

//标记是否更改过
int changed = 0;
//设置是否自动show 
int auto_show = 1;

//argc放着指令word的个数 
//argv[0]放着"/editor",argv[1]存放着文件名字 
int main(int argc, char *argv[])
{
	if (argc == 1)
	{
		printf(1, "Please enter the instructions like [editor file_name]\n");
		exit();
	}
	
	char *text[MAX_LINE_NUMBER] = {};// 存放文件的内容(必须这样声明否则用不了memset) 
	text[0] = malloc(MAX_LINE_LENGTH);//
	memset(text[0], 0, MAX_LINE_LENGTH);//初始化操作(将第一行全部填0) 
	//存储当前最大的行号，从0开始。即若line_number == x，则从text[0]到text[x]可用
	int line_number = 0;
	//打开文件并读取里面的内容
	int fd = open(argv[1], O_CREATE);//文件不存在时会自动创建新的文件 
	printf(1, "File open successfully\n");
	char buf[BUF_SIZE] = {};//存放输入的字符串 
	int len = 0;
	//把文件内容存放到text中 
	while ((len = read(fd, buf, BUF_SIZE)) > 0)
	{
		int i = 0;
		int next = 0;
		int is_full = 0;// 
		while (i < len)
		{
			for (i = next; i < len && buf[i] != '\n'; i++)//让i走到字符串最后 
				;
			strcat(text[line_number], buf+next, i-next);//拷贝"\n"之前的内容
			//继续在text新建一行
			if (i < len && buf[i] == '\n')
			{
				if (line_number >= MAX_LINE_NUMBER - 1)
					is_full = 1;
				else
				{
					line_number++;
					//在text新建一行
					text[line_number] = malloc(MAX_LINE_LENGTH);
					//初始化操作(将这一行全部填0) 
					memset(text[line_number], 0, MAX_LINE_LENGTH);
				}
			}
			if (is_full == 1 || i >= len - 1)
				break;
			else
				next = i + 1;
		}
		if (is_full == 1)
			break;
	}
	close(fd);
	
	//输出文件内容
	show_text(text);
	
	//处理命令
	char input[MAX_LINE_LENGTH] = {};//输入的指令 
	while (1)
	{
		printf(1, "\nPlease enter an instruction:\n");
		memset(input, 0, MAX_LINE_LENGTH);
		gets(input, MAX_LINE_LENGTH-1);//只能输入255位 
		int len = strlen(input);
		input[len] = '\0';//最后一位置'\0' 
		
		//ins
		if (input[0] == 'i' && input[1] == 'n' && input[2] == 's')
		{
			if (input[3] == '-'&&atoi(&input[4])>=0)
			{
				text_ins(text, atoi(&input[4])-1);//插入 
				line_number = get_line_number(text);//更新行号
			}
			else if(input[3] == ' '||input[3] == '\0'||input[3] == '\n')
			{
				text_ins(text, line_number);
                line_number = get_line_number(text);
//               printf(2,"%d\n",line_number);
			}
			else
			{
				printf(1, "invalid instruction.\n");
				text_help(text);
			}
		}
		//mod
		else if (input[0] == 'm' && input[1] == 'o' && input[2] == 'd')
		{
			if (input[3] == '-'&&atoi(&input[4])>=0){
				text_mod(text, atoi(&input[4]));
				line_number = get_line_number(text);
			}
			else if(input[3] == ' '||input[3] == '\0'||input[3] == '\n'){
				text_mod(text, line_number);
				line_number = get_line_number(text);
			}
			else
			{
				printf(1, "invalid instruction.\n");
				text_help(text);
			}
		}
		//del
		else if (input[0] == 'd' && input[1] == 'e' && input[2] == 'l')
		{
			if (input[3] == '-'&&atoi(&input[4])>=0)
			{
				text_del(text, atoi(&input[4]));
				line_number = get_line_number(text);
			}	
			else if(input[3]=='\0'||input[3] == '\n')
			{
				text_del(text, line_number);
				line_number = get_line_number(text);
			}
			else
			{
				printf(1, "Invalid instruction.\n");
				text_help(text);
			}
			
		}
		else if (strcmp(input, "showtext") == 0)
		{
			show_text(text);
		}
		else if (strcmp(input, "show") == 0)
		{
			auto_show = 1;
			printf(1, "Enable show current contents after text changed.\n");
		}
		else if (strcmp(input, "hide") == 0)
		{
			auto_show = 0;
			printf(1, "Disable show current contents after text changed.\n");
		}
		else if (strcmp(input, "help") == 0)
			text_help(text);
		else if (strcmp(input, "save") == 0 || strcmp(input, "CTRL+S\n") == 0)
			text_save(text, argv[1]);
		else if (strcmp(input, "exit") == 0)
			text_exit(text, argv[1]);
		else
		{
			printf(1, "Invalid instruction.\n");
			text_help(text);
		}
	}
	exit();
}

void show_text(char *text[])
{
	printf(1, "********************************************\n");
	printf(1, "The content of the file is:\n");
	int j = 0;
	//前三个%d表示行号，text[j]为此行内容 
	for (; text[j] != NULL; j++)
		printf(1, "%d%d%d:%s\n", (j+1)/100, ((j+1)%100)/10, (j+1)%10, text[j]);
	printf(1, "********************************************\n");
}

//获取当前最大的行号，从0开始，范围text[0]到text[x]
int get_line_number(char *text[])
{
	int i = 0;
	for (; i < MAX_LINE_NUMBER; i++)
		if (text[i] == NULL)
			return i - 1;
	return i - 1;
}

//插入命令，n为用户输入的行号，从1开始
void text_ins(char *text[], int n)
{
	if (n < 0 || n > get_line_number(text))
	{
		printf(1, "Invalid line number\n");
		return;
	}
	if( get_line_number(text) >= MAX_LINE_LENGTH - 2)//判断文件是否满了 (此处减2是调试出来的)
	{
		printf(1, "The file is full!\n");
		return;
	}
	char input[MAX_LINE_LENGTH] = {};
	printf(1, "Please input the content:\n");
	gets(input, MAX_LINE_LENGTH-1);
	input[strlen(input)] = '\0';
	
	int i = MAX_LINE_NUMBER - 1;//从最后往前找 
	for (; i > n; i--)//一直到用户的输入的第n行结束 
	{
		if (text[i-1] == NULL)//当前行为空便继续向前 
			continue;
		//当前行为空，上一行不为空，为当前行开辟空间，把上一行放到当前行 
		else if (text[i] == NULL && text[i-1] != NULL)
		{
			text[i] = malloc(MAX_LINE_LENGTH);
			memset(text[i], 0, MAX_LINE_LENGTH);
			strcpy(text[i], text[i-1]);
		}
		//当前行不为空，上一行也不为空，把上一行放到当前行 
		else if (text[i] != NULL && text[i-1] != NULL)
		{
			memset(text[i], 0, MAX_LINE_LENGTH);
			strcpy(text[i], text[i-1]);
		}
	}
	if (text[n] == NULL)//若此行为空，先开辟空间 
	{
		text[n] = malloc(MAX_LINE_LENGTH);
	}
	memset(text[n], 0, MAX_LINE_LENGTH);//清零 
	strcpy(text[n], input);//把内容写上去 
	changed = 1;
	if (auto_show == 1)
		show_text(text);
}

//修改命令，n为用户输入的行号，从1开始
void text_mod(char *text[], int n)
{
	if (n <= 0 || n > get_line_number(text) )
	{
		printf(1, "Invalid line number\n");
		return;
	}
	char input[MAX_LINE_LENGTH] = {};
	printf(1, "Please input content:\n");
	gets(input, MAX_LINE_LENGTH-1);
	input[strlen(input)] = '\0';
	
	memset(text[n-1], 0, MAX_LINE_LENGTH);
	strcpy(text[n-1], input);
	changed = 1;
	if (auto_show == 1)
		show_text(text);
}

//删除命令，n为用户输入的行号，从1开始
void text_del(char *text[], int n)
{
	if (n <= 0 || n > get_line_number(text) )
	{
		printf(1, "Invalid line number\n");
		return;
	}
	memset(text[n-1], 0, MAX_LINE_LENGTH);//把此行清零 
	int i = n - 1;
	//循环用text[i+1]覆盖text[i] 
	for (; text[i+1] != NULL; i++)
	{
		strcpy(text[i], text[i+1]);
		memset(text[i+1], 0, MAX_LINE_LENGTH);
	}
	//如果是第一行并且后面没有内容则不释放空间 
	if (i != 0)
	{
		free(text[i]);
		text[i] = 0;
	}
	changed = 1;
	if (auto_show == 1)
		show_text(text);
}

void text_help(char *text[])
{
	printf(1,"********************************************\n");
	printf(1, "Instructions for user:\n");
	printf(1, "ins-n, insert a line after line n\n");
	printf(1, "mod-n, modify line n\n");
	printf(1, "del-n, delete line n\n");
	printf(1, "ins, insert a line after the last line\n");
	printf(1, "mod, modify the last line\n");
	printf(1, "del, delete the last line\n");
	printf(1, "show, enable show current contents after executing an instruction.\n");
	printf(1, "hide, disable show current contents after executing an instruction.\n");
	printf(1, "save, save the file\n");
	printf(1, "exit, exit editor\n");
	printf(1,"********************************************\n");
}

void text_save(char *text[], char *path)
{
	//删除旧有文件
	unlink(path);//in system call 
	//新建文件并打开
	//O_WRONLY参数为写文件  O_CREATE参数为创建文件 
	int fd = open(path, O_WRONLY|O_CREATE);
	if (fd == -1)
	{
		printf(1, "Save failed, file can't open:\n");
		exit();
	}
	if (text[0] == NULL)
	{
		close(fd);
		return;
	}
	//写数据
	//write函数，把text[0]中长度为len的写进文件fd中 
	write(fd, text[0], strlen(text[0]));
	int i = 1;
	for (; text[i] != NULL; i++)
	{
		printf(fd, "\n");//写入'\n' 
		write(fd, text[i], strlen(text[i]));//写入text内容 
	}
	close(fd);
	printf(1, "Saved successfully\n");
	changed = 0;
	return;
}

void text_exit(char *text[], char *path)
{
	//询问是否保存
	while (changed == 1)
	{
		printf(1, "Save the file? y/n\n");
		char input[MAX_LINE_LENGTH] = {};
		gets(input, MAX_LINE_LENGTH-1);
		input[strlen(input)] = '\0';
		if (strcmp(input, "y") == 0)
			text_save(text, path);
		else if(strcmp(input, "n") == 0)
			break;
		else
			printf(1, "Invalid answer!\n");
	}
	//释放内存
	int i = 0;
	for (; text[i] != NULL; i++)
	{
		free(text[i]);
		text[i] = 0;
	}
	exit();
}

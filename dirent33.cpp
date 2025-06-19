// #define _GNU_SOURCE
#include <linux/stat.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <pwd.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <linux/kernel.h>
#include <cstring>
#include <time.h>
// AT_STATX_SYNC_AS_STAT

using namespace std;


/*                      
               struct statx_timestamp stx_atime;  
               struct statx_timestamp stx_btime;  
               struct statx_timestamp stx_ctime;  
  */

void   permissions(const char * file,struct stat * a)
{
        //struct stat st;
        char perm1[10];
    if(stat(file, a) == 0){
//         printf("PERMISSION ARE READABLE\n");
	 mode_t perm = a->st_mode;
        perm1[0] = (perm & S_IRUSR) ? 'r' : '-';
        perm1[1] = (perm & S_IWUSR) ? 'w' : '-';
        perm1[2] = (perm & S_IXUSR) ? 'x' : '-';
        perm1[3] = (perm & S_IRGRP) ? 'r' : '-';
        perm1[4] = (perm & S_IWGRP) ? 'w' : '-';
        perm1[5] = (perm & S_IXGRP) ? 'x' : '-';
        perm1[6] = (perm & S_IROTH) ? 'r' : '-';
        perm1[7] = (perm & S_IWOTH) ? 'w' : '-';
        perm1[8] = (perm & S_IXOTH) ? 'x' : '-';
        perm1[9] = '\0';
         printf("%s ", perm1);
        return;
    }
  //  else{
    //    return perm1;
   // }
}


static void print_time(const char *field, struct statx_timestamp *ts)
{
	struct tm tm;
	time_t tim;
	char buffer[100];
	int len;

	tim = ts->tv_sec;
	if (!localtime_r(&tim, &tm)) {
		perror("localtime_r");
		_exit(1);
	}
	len = strftime(buffer, 100, "%F %T", &tm);
	if (len == 0) {
		perror("strftime");
		_exit(1);
	}
//	printf("%s", field);
//	fwrite(buffer, 1, len, stdout);
	//printf(".%09u", ts->tv_nsec);
//	len = strftime(buffer, 100, "%z", &tm);
	if (len == 0) {
		perror("strftime2");
		_exit(1);
	}
	fwrite(buffer, 1, len, stdout);
	//printf("\n");
}

ssize_t statx_func(int dfd, const char * filename, unsigned flags,
		   unsigned int mask, struct statx * buffer)
{
		return syscall(__NR_statx, dfd, filename, flags, mask, buffer);
}

struct linux_dirent {
	unsigned long d_ino;
	off_t d_off;
	unsigned short d_reclen;
	char d_name[];
};
#define BUF_SIZE 1024

int main()
{	    struct dirent * ld;
	    char * buf;
	    int atflag;
	    struct statx statbuf;
	    struct passwd * user;
	    struct passwd * group;
	    struct statx_timestamp atime2;
	    struct stat a;
		
	    atflag &= ~AT_STATX_SYNC_TYPE;
	    atflag |= AT_STATX_FORCE_SYNC;


	    DIR * d = opendir(".");
	    int fd = dirfd(d);
	
		while((ld = readdir(d)) != NULL)
		{	const char * fileNameString =  ld->d_name;  	
			int gd = openat(AT_FDCWD,fileNameString, O_RDONLY | O_CLOEXEC);
			int ret = statx(fd,ld->d_name,atflag,STATX_ALL ,&statbuf);
			close(gd);
			printf("..\n");
			user = getpwuid(statbuf.stx_uid);
			group  = getpwuid(statbuf.stx_gid); 	
			struct timespec ts;
        		atime2 = statbuf.stx_atime;
                        const char * field;
        		permissions(fileNameString,&a);
			printf(" %s %s ", user->pw_name, group->pw_name);
		//	print_time(field,&atime2);
			printf(" %s", fileNameString); 
			printf("\n");		
			
			 }
		


    	 
	//close(fd);	

	return 1;


}

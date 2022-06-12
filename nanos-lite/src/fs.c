#include "fs.h"

typedef struct {
  char *name;
  size_t size;
  off_t disk_offset;
  off_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO, FD_NORMAL};

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin (note that this is not the actual stdin)", 0, 0},
  {"stdout (note that this is not the actual stdout)", 0, 0},
  {"stderr (note that this is not the actual stderr)", 0, 0},
  [FD_FB] = {"/dev/fb", 0, 0},
  [FD_EVENTS] = {"/dev/events", 0, 0},
  [FD_DISPINFO] = {"/proc/dispinfo", 128, 0},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

extern _Screen _screen;
void init_fs() {
  // TODO: initialize the size of /dev/fb
	///3
	file_table[FD_FB].size=4*_screen.height*_screen.width;
}



int fs_open(const char *path,int flag,int mode){
	Log("%s",path);
	for(int i=0;i<NR_FILES;i++){
		if(strcmp(path,file_table[i].name)==0){
			file_table[i].open_offset=0;
			return i;
		}
	}
	assert(0);
	return -1;	
}


size_t fs_filesz(int fd){
	return file_table[fd].size;
}

extern void dispinfo_read(void *buf, off_t offset, size_t len);
extern void ramdisk_read(void*,off_t,size_t);
extern size_t events_read(void *buf, size_t len);
ssize_t fs_read(int fd,void *buf,size_t count){
    off_t open_offset=file_table[fd].open_offset;
	size_t size=file_table[fd].size;	
	if(fd==FD_DISPINFO){
		count=(open_offset+count)<=size?count:size-open_offset;
		dispinfo_read(buf,open_offset,count);
		//Log("%s",buf);
		file_table[5].open_offset+=count;
		return count;
        }
	if(fd==	FD_EVENTS){
	return events_read(buf,count);
	}
	count=(open_offset+count)<=size?count:size-open_offset;
	//Log("%d",count);
	//Log("%d",size);	
	ramdisk_read(buf,file_table[fd].disk_offset+open_offset,count);
	file_table[fd].open_offset=count+open_offset;
	return count;	
	
}

extern void fb_write(const void *buf, off_t offset, size_t len);
extern void ramdisk_write(const void*,off_t,size_t);
ssize_t fs_write(int fd,void*buf,size_t count){
   	off_t open_offset=file_table[fd].open_offset;
	size_t size=file_table[fd].size;
	if(fd==1||fd==2){
		char*temp=(char*)buf;
		for(int i=0;i<count;i++){
		_putc(*(temp+i));
		}
		return count;	
	}
        else if(fd==FD_FB){
	      count=(open_offset+count)<=size?count:size-open_offset;	
              fb_write(buf,open_offset,count);
	      file_table[3].open_offset=open_offset+count;
              return count;	
	}
	else{
     count=(open_offset+count)<=size?count:size-open_offset;		
	 ramdisk_write(buf,file_table[fd].disk_offset+open_offset,count);
		file_table[fd].open_offset=open_offset+count;
		return count;			
		
	
	}
}


off_t fs_lseek(int fd,off_t offset,int whence){
	switch(whence){
		case SEEK_SET:
				file_table[fd].open_offset=offset;
				return offset;
				break;
		case SEEK_CUR:
				file_table[fd].open_offset+=offset;
				return file_table[fd].open_offset;
				break;
		case SEEK_END:
				file_table[fd].open_offset=offset+file_table[fd].size;
				return file_table[fd].open_offset;
				break;	
		default:return offset-1;
	}
}






/*
	Name：		GCG-filesorter
	Author:		Chise Hachiroku (C86.moe)
	Email:		i@c86.moe
	Description:This program can select changed items in by comparing with an older copy and
				copying the changed ones to a new folder.
	Useage:		<program name> [New copy] [Destination of changed items] [Older copy]
*/

// Listing all possible headers. May not use them all.
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<unistd.h>
#include<string.h>
#include<strings.h>
#include<errno.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<dirent.h>
#include<time.h>

// Define ahead to avoid some certain issues.
int is_dir( char* file_name);

// Check if modified, and copy if it is.
void cp_file( char *source_path , char *destination_path, char* compare_path)
{
	// Construct compare command and execute it.
	char command[1030];
	sprintf(command,"diff -q --speed-large-files \"%s\" \"%s\"",source_path,compare_path);
	if(system(command)==0){
		printf("%s is no new. Skip.\n", source_path);
		return;
	}

	// Open files.
	FILE* fp_src = NULL;
	FILE* fp_dst = NULL;
	if((fp_src = fopen(source_path,"r"))==NULL){
		printf("Error in opening %s. Abort.\n", source_path);
		exit(1);
	}
	if((fp_dst=fopen(destination_path,"w"))==NULL){
		printf("Error in creating %s. Abort.\n", destination_path);
		exit(1);
	}

	// Copying. Not very efficient indeed.
	int c;
	while(1) // A dead loop indeed. May work better in this case.
	{
		c = fgetc(fp_src);
		if( c == EOF && feof(fp_src))
		{
			break;
		}
		else if( ferror(fp_src))
		{
			perror("fget()");
			break;
		}
		fputc(c,fp_dst);
	} 
	fclose(fp_src); 
	fclose(fp_dst); 
}

int endwith(char* s,char c){
	if(s[strlen(s)-1]==c){
		return 1;
	}
	else{
		return 0;
	}
}

// List every dir and file.
void copy_folder(char* source_path, char* destination_path, char* compare_path)
{
	DIR *dst_dp = opendir(destination_path);
	if(dst_dp  == NULL)
	{
		printf("Working on dir %s.\n", source_path);
		if(mkdir(destination_path,0777) == -1)
		{
			printf("Error in creating dir. Abort.\n");
			exit(-1);
		}
	}
	else{
		printf("Destination dir exists. May overwrite existing data. Abort.\n");
		// Considering the process, it acts as an insurence.
		exit(2);
	}
	DIR *src_dp = opendir(source_path);	
	struct dirent *ep_src =  readdir(src_dp);
	char address[512] = {0};
	char toaddress[512] = {0};
	char compaddress[512] = {0};
	while(1)
	{
		sprintf(address,"%s/%s",source_path,ep_src->d_name);
		sprintf(toaddress,"%s/%s",destination_path,ep_src->d_name);
		sprintf(compaddress,"%s/%s",compare_path,ep_src->d_name);
		if(endwith(address,'.') == 1)
		{
			//In this case, it must be either this directory or its ancestor.	
		}
		else if( ( is_dir(address) != 1) )//if the file is not dir just copy file
		{
			cp_file(address,toaddress,compaddress);	
		}
		else
		{
			// This item is a dir , call copy_folder function again.
			copy_folder(address,toaddress,compaddress);
		}

		// All files copied?
		if((ep_src = readdir(src_dp)) == NULL )
			break;	

		// Housekeeping for next round.
		memset(address,sizeof(address),0);
		memset(toaddress,sizeof(toaddress),0);
		memset(compaddress,sizeof(compaddress),0);
	}
	    closedir(dst_dp);
		closedir(src_dp);
		// Sometimes no action is done during the process, delete empty folder.
		char command[520];
		sprintf(command, "rmdir \"%s\"", destination_path);
		system(command);
		return;
}
int is_dir( char* file_name)
{
	struct stat info;
	stat(file_name,&info);
	if(S_ISDIR(info.st_mode))
		return 1;
	else
		return 0;
}

int main( int argc , char** argv)
{
	printf("(GCG) Differented files extractor by i@C86.moe\n");
	if(argc != 4)
	{
		printf("Usage: %s <Origin Dir> <Target Dir> <Old Dir>\n",argv[0]);
		exit(1);
	}
	copy_folder(argv[1],argv[2],argv[3]);
	printf("Done.\n");
	return 0;
}
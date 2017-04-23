#include <sys/stat.h>
#include <stdio.h>  //printf
#include <stdlib.h>  //atoi
#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <string.h>  // strcmp()

using namespace std;

int inode_num=-1;
double min_size=-1.0, max_size=-1.0;
string name=" ";
bool inode_config=false, name_config=false, size_min_config=false, size_max_config=false;

bool isDir(string name)
{
    struct stat buf;
    stat(name.c_str(), &buf);
    if( name !=  "." && name != ".."&& (S_ISDIR(buf.st_mode)))
        return true;
    else
    {
        return false;
    }
}



int scanDir(string dname)
{
    /** open the directory stream and
     *  go through the whole dir
     */
    
    DIR *dp = NULL;     //directory pointer
    struct dirent *dptr = NULL;     //the infomation about file in the dir
    string prefix = dname;

    if(NULL == (dp = opendir(dname.c_str()))) 
    {
        cerr<< "\n ***Can not open \"";
        cerr<<dname;
        cerr << "\" directory***\n";
        return -1;
    }

    while(NULL != (dptr = readdir(dp)))
    {
        if( strcmp(dptr->d_name, ".") && strcmp(dptr->d_name, "..") )
        {
            struct stat buf2;
            string path = dname+"/"+dptr->d_name;
            stat( path.c_str(), &(buf2));
            double size_MB = (buf2.st_size*1.0)/(1024.0*1024.0); 
            if( (!name_config || !strcmp(dptr->d_name, name.c_str()))&&( !inode_config || inode_num==buf2.st_ino)&&( !size_max_config || max_size>=size_MB)&&( !size_min_config || min_size<=size_MB) )
            {
                cout<<path<<" ";
                cout<<buf2.st_ino<<" ";
                cout<<size_MB<<" MB"<<endl;
            }
            if( isDir(path) ){
                scanDir(dname+"/"+dptr->d_name);
            }
        }
    }

    /** close the directory stream
    */
    closedir(dp);
    return 1;
}

int main(int argc, char **argv)
{
    /**First, Check argc bigger than 1 ,and then check the valid options:
     * 1. -inode <number>
     * 2. -name <number>
     * 3. -size_min <size of MB>
     * 4. -size_max <size of MB>
     */    
    

    // argv[0]: program name, [1]: dir name , [2~]: option
    if(argc <= 1)
    {
        cerr << "You mustt input the directory path"<<endl;
        return -1;
    }

    //parse path, remove the end "/"
    if(argv[1][strlen(argv[1])-1] == '/')
    {
        argv[1][strlen(argv[1])-1]='\0';
    }

    for(int i=2;i<argc;i+=2)
    {
        if( ! strcmp(argv[i], "-inode"))
        {
            inode_config = true;
            inode_num = atoi(argv[i+1]);
        }
        else if( ! strcmp(argv[i], "-name"))
        {
            name_config = true;
            name = argv[i+1];
        }
        else if( ! strcmp(argv[i], "-size_min"))
        {
            size_min_config = true;
            min_size = atof(argv[i+1]);
        }
        else if( ! strcmp(argv[i], "-size_max"))
        {
            size_max_config = true;
            max_size = atof(argv[i+1]);
            //cout<<"----"<<max_size<<"------"<<endl;
        }else
        {
            cerr<< "invalid option !!!"<<endl;
            return -1;
        }

    }
    /* end of option check */

    scanDir(argv[1]);

}

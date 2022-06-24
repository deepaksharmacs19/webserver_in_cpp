#include<tmwp>
#include<windows.h>
#include<stdio.h>
#include<string.h>
#include<iostream>
#include<stdlib.h>

using namespace tmwp;
void Request :: setKeyValue(string k,string v)
{
this->keyValues.insert(pair<string,string>(k,v));
}

string Request :: getValue(string k)
{
string v;
map<string,string> :: iterator i=keyValues.begin();
i=keyValues.find(k);
if(i==keyValues.end()) v="";
else v=i->second;
return v;
}

int extensionEquals(const char *left,const char *right)
{
char a,b;
while(*left && *right)
{
a=*left;
b=*right;
if(65<=a && a>=90) a+=32;
if(65<=b && b>=90) b+=+32;
if(a!=b)return 0;
left++;
right++;
}
return *left==*right;
}

char * getMIMEType(char *resource)
{
char *mimeType=NULL;
int len=strlen(resource);
if(len<4)return mimeType;
int lastIndexOfDot=len-1;
while(lastIndexOfDot &&  resource[lastIndexOfDot]!='.')lastIndexOfDot--;
if(lastIndexOfDot==0)return mimeType;
if(extensionEquals(resource+lastIndexOfDot+1,"html"))
{
mimeType=(char*)malloc(sizeof(char)*10);
strcpy(mimeType,"text/html");
}
if(extensionEquals(resource+lastIndexOfDot+1,"css"))
{
mimeType=(char*)malloc(sizeof(char)*9);
strcpy(mimeType,"text/css");
}
if(extensionEquals(resource+lastIndexOfDot+1,"js"))
{
mimeType=(char*)malloc(sizeof(char)*16);
strcpy(mimeType,"text/javascript");
}
if(extensionEquals(resource+lastIndexOfDot+1,"jpg"))
{
mimeType=(char*)malloc(sizeof(char)*11);
strcpy(mimeType,"image/jpeg");
}
if(extensionEquals(resource+lastIndexOfDot+1,"jpeg"))
{
mimeType=(char*)malloc(sizeof(char)*11);
strcpy(mimeType,"image/jpeg");
}

if(extensionEquals(resource+lastIndexOfDot+1,"png"))
{
mimeType=(char*)malloc(sizeof(char)*10);
strcpy(mimeType,"image/png");
}

if(extensionEquals(resource+lastIndexOfDot+1,"ico"))
{
mimeType=(char*)malloc(sizeof(char)*13);
strcpy(mimeType,"image/x-icon");
}
return mimeType;
}

char isClientSideResourse(char *resource)
{
int i;
for(i=0;resource[i]!='\0' && resource[i]!='.';i++);
if(resource[i]=='\0')return 'N';
return 'Y'; // this will have to be changed latter on
}

Request * parseRequest(char *bytes)
{
char method[11];
char resource[1001];
int i,j;
for(i=0;bytes[i]!=' ';i++)method[i]=bytes[i];
method[i]='\0';
i+=2;
char **data=NULL;
int dataCount=0;

if(strcmp(method,"GET")==0)
{
for(j=0;bytes[i]!=' ';i++,j++)
{
if(bytes[i]=='?')break;
resource[j]=bytes[i];
}
resource[j]='\0';
if(bytes[i]=='?')
{
i++;
int si=i;
dataCount=0;
while(bytes[i]!=' ')
{
if(bytes[i]=='&')dataCount++;
i++;
}
dataCount++;
data=(char **)malloc(sizeof(char *)*dataCount);
int *pc=(int *)malloc(sizeof(int)*dataCount);
i=si;
int j=0;
while(bytes[i]!=' ')
{
if(bytes[i]=='&')
{
pc[j]=i;
j++;
}
i++;
}
pc[j]=i;
i=si;
j=0;
int howManyTopick;
while(j<dataCount)
{
howManyTopick=pc[j]-i;
data[j]=(char*)malloc(sizeof(char)*(howManyTopick+1));
strncpy(data[j],bytes+i,howManyTopick);
data[j][howManyTopick]='\0';
i=pc[j]+1;
j++;
}
}
}// method is of GET type


printf("Request arrived for %s\n",resource);

Request *request=new Request;
request->method=(char*)malloc(sizeof(char)*strlen(method)+1);
request->dataCount=dataCount;
request->data=data;


strcpy(request->method,method);

if(resource[0]=='\0')
{
request->resource=NULL;
request->isClientSideTechnologyResourse='Y';
request->mimeType=NULL;
}
else
{
request->resource=(char*)malloc(sizeof(char)*strlen(resource)+1);
strcpy(request->resource,resource);
request->isClientSideTechnologyResourse=isClientSideResourse(resource);
request->mimeType=getMIMEType(resource);
}
printf("%s",request->resource);
return request;
}

TMWebProjector::TMWebProjector(int portNumber)
{
this->portNumber=portNumber;
}

TMWebProjector::~TMWebProjector()
{
// if(this->url) delete [] this->url;
}

Response :: Response(int clientSocketDescriptor)
{
this->clientSocketDescriptor=clientSocketDescriptor;
this->isClose=false;
this->headerCreated=false;
}
void Response :: createHeader()
{
char header[51];
strcpy(header,"HTTP/1.1 200 OK\nContent-Type:text/html\n\n");
send(clientSocketDescriptor,header,strlen(header),0);
this->headerCreated=true;
}

void Request :: forward(string forwardTo)
{
if(forwardTo.length()<=0) return;
this->forwardTo=forwardTo;
}

void Response :: write(const char * str)
{
if(str==NULL) return;
int len=strlen(str);
if(len==0) return;
if(!this->headerCreated) createHeader();
send(clientSocketDescriptor,str,len,0);
}

void Response:: close()
{
if(isClose) return;
closesocket(clientSocketDescriptor);
}

string Request :: Encoder(string v)
{
string val=v;
for(int i=0;i<val.length();i++)
{
if(val[i]==' ') val[i]='+'; 
if(val[i]==39)val.replace(i,1,"%27");
if(val[i]=='/')val.replace(i,1,"%2F");
if(val[i]==',')val.replace(i,1,"%2C");
if(val[i]=='?')val.replace(i,1,"%3F");
}
return val;
}

string Request::Decoder(string v)
{
string val=v;
for(int i=0;i<val.length();i++)
{
if(val[i]=='+') val[i]=' '; 
if(val[i]=='%' && val[i+1]=='2' && val[i+2]=='7')val.replace(i,3,"'");
if(val[i]=='%' && val[i+1]=='2' && val[i+2]=='F')val.replace(i,3,"/");
if(val[i]=='%' && val[i+1]=='2' && val[i+2]=='C')val.replace(i,3,",");
if(val[i]=='%' && val[i+1]=='3' && val[i+2]=='F')val.replace(i,3,"?");
}
return val;
}

string Request::get(string name)
{
string val;
int e,i;
for(i=0;i<this->dataCount;i++)
{
for(e=0;data[i][e]!='\0' && data[i][e]!='=';e++);
if(data[i][e]!='=')continue;
if(strncmp(data[i],name.c_str(),e)==0) break;
}
if(i==dataCount)
{
val="";
}
else 
{
val=this->Decoder(string(data[i]+(e+1)));
}
return val;
}

void Response :: write(string str)
{
this->write(str.c_str());
}

void TMWebProjector::onRequest(string url,void (*ptrOnRequest)(Request &,Response &))
{
if(url.length()<=0 || ptrOnRequest==NULL)return;
requestMappings.insert(pair<string,void (*)(Request&,Response &)>(url,ptrOnRequest));
}


int Request :: isTPL(char *str)
{
if(str==NULL) return 0;
int i;
for(i=0;str[i]!='\0' && str[i]!='.';i++);
if(str[i]=='\0') return 0;
i++;
if(extensionEquals(str+i,"tpl"))return 1; 
return 0;
}

void Request :: changeTPL(char *str,int csd)
{
int i=0;
char arr[1001];
FILE *f;
f=fopen(str,"r");
char a;
while(1)
{
if(!feof(f)) break;
fread(&a,sizeof(char),1,f);
arr[i]=a;
i++;
}
fclose(f);
arr[i]='\0';
//cout<<"arr[i]"<<arr[i-1]<<endl;
//arr[i-3]='\0';
int e=0;
char key[101];
char reFile[1001];
int size=strlen(arr);
int j=0;
i=0;
while(i!=size)
{
reFile[e]=arr[i];
if(arr[i]=='$' && arr[i+1]=='$' & arr[i+2]=='$' && arr[i+3]=='{')
{
i+=4;
while(arr[i]!='}')
{
key[j]=arr[i];
i++;
j++;
}
key[j]='\0'
;break;
}
reFile[e]=arr[i];
i++;
e++;
}
cout<<key<<endl;
//done done
string v;
map<string,string> :: iterator iter=keyValues.begin();
iter=keyValues.find(string(key));
if(iter==keyValues.end()) return;
else v=iter->second;
cout<<"The second value of iterator : "<<iter->second<<endl;
j=0;
while(j<v.length())
{
reFile[e]=v[j];
e++;
j++;
}
i++;
while(i!=size)
{
reFile[e]=arr[i];
cout<<reFile[e]<<endl;
i++;
e++;
}
reFile[e]='\0';
Response res(csd);
res.write(reFile);
//f=fopen(str,"w");
//fwrite(&reFile,sizeof(char *),strlen(reFile),f);
//fclose(f);
printf("%s\n",reFile);
}

void TMWebProjector::start()
{
int length;
int i,rc;
char g;
FILE *f;
char requestBuffer[1024]; // A chunk of 1024 + 1 space for string
char responseBuffer[8192]; // 8192 1024 x 8

WSADATA wsaData;
WORD var;
int serverSocketDescriptor;
int clientSocketDescriptor;

struct sockaddr_in serverSocketInformation;
struct sockaddr_in clientSocketInformation;

int bytesExtracted;

int successCode;
int len;

var=MAKEWORD(1,1);
WSAStartup(var,&wsaData);

serverSocketDescriptor=socket(AF_INET,SOCK_STREAM,0);
if(serverSocketDescriptor<0)
{
printf("unable to create socket\n");
return;
}

serverSocketInformation.sin_family=AF_INET;
serverSocketInformation.sin_port=htons(this->portNumber);
serverSocketInformation.sin_addr.s_addr=htonl(INADDR_ANY);

successCode=bind(serverSocketDescriptor,(struct sockaddr *)&serverSocketInformation,sizeof(serverSocketInformation));
char message[101];

if(successCode<0)
{
sprintf(message,"unable to bind socket to port %d",this->portNumber);
printf("%s\n",message);
WSACleanup();
return;
}
listen(serverSocketDescriptor,10);
len=sizeof(clientSocketInformation);
while(1)
{
sprintf(message,"TMServer is ready to accept request on port %d",this->portNumber);
printf("%s\n",message);
clientSocketDescriptor=accept(serverSocketDescriptor,(struct sockaddr *)&clientSocketInformation,&len);
if(clientSocketDescriptor<0)
{
printf("unable to accept client to connection\n");
closesocket(serverSocketDescriptor);
WSACleanup();
return;
}
bytesExtracted=recv(clientSocketDescriptor,requestBuffer,sizeof(requestBuffer),0);
if(bytesExtracted<0)
{
// what to do is yet to be decided
}else 
if(bytesExtracted==0)
{
// what to do is yet to be decided
}
else
{
requestBuffer[bytesExtracted]='\0';
Request *request=parseRequest(requestBuffer);

while(1)
{
if(request->isClientSideTechnologyResourse=='Y')
{
if(request->isTPL(request->resource)) 
{
request->changeTPL(request->resource, clientSocketDescriptor);
break;
}
if(request->resource==NULL)
{
f=fopen("index.html","rb");
if(f!=NULL)printf("Sending index.html\n");
if(f==NULL)
{
f=fopen("index.htm","rb");
if(f!=NULL)printf("Sending index.htm\n");
}
if(f==NULL)
{
strcpy(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:163\n\n<!DOCTYPE HTML><html lang='en'><head><meta charset='utf-8'><title>TM Web Projector</title></head><body><h2 style='color:red'>Resourse / not found</h2></body></html>");
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);
printf("Sending 404 page\n");
break;
}
else
{
fseek(f,0,2); // move the internal pointer to the end of file
length=ftell(f);
fseek(f,0,0); // move the internal pointer to the start file
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:%s\nContent-Length:%d\nKeep-Alive: timeout=5, max=1000\n\n",request->mimeType,length);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);
i=0;
while(i<length)
{
rc=length-i;
if(rc>1024) rc=1024;
fread(&responseBuffer,rc,1,f);
send(clientSocketDescriptor,responseBuffer,rc,0);
i+=rc;
}
fclose(f);
closesocket(clientSocketDescriptor); 
break;
} 
} 
else
{
f=fopen(request->resource,"rb");
if(f!=NULL)printf("Sending %s\n",request->resource);
if(f==NULL)
{
char tmp[501];
printf("Sending 404 page\n");
sprintf(tmp,"<!DOCTYPE HTML><html lang='en'><head><meta charset='utf-8'><title>TM Web Projector</title></head><body><h2 style='color:red'>Resourse /%s not found</h2></body></html>",request->resource);
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\n\n",strlen(tmp));
strcat(responseBuffer,tmp);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);
break;
}
else
{
fseek(f,0,2); // move the internal pointer to the end of file
length=ftell(f);
fseek(f,0,0); // move the internal pointer to the start file
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:%s\nContent-Length:%d\nKeep-Alive: timeout=5, max=1000\n\n",request->mimeType,length);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);
i=0;
while(i<length)
{
rc=length-i;
if(rc>1024) rc=1024;
fread(&responseBuffer,rc,1,f);
send(clientSocketDescriptor,responseBuffer,rc,0);
i+=rc;
}
fclose(f);
closesocket(clientSocketDescriptor);
break;
} 
} 
}
else
{
// what to do in case of server side resouce is yet to be decided

map<string,void(*)(Request &,Response&)> :: iterator iter;
iter=requestMappings.find(string("/")+string(request->resource));

if(iter==requestMappings.end())
{
char tmp[501];
printf("Sending 404 page\n");
sprintf(tmp,"<!DOCTYPE HTML><html lang='en'><head><meta charset='utf-8'><title>TM Web Projector</title></head><body><h2 style='color:red'>Resourse /%s not found</h2></body></html>",request->resource);
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\n\n",strlen(tmp));
strcat(responseBuffer,tmp);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);
break;
}
else
{
Response resp(clientSocketDescriptor);
iter->second(*request,resp);
if(request->forwardTo.length()>0)
{
free(request->resource);
request->resource=(char*)malloc(sizeof(char)*(request->forwardTo.length()+1));
strcpy(request->resource,request->forwardTo.c_str());
request->isClientSideTechnologyResourse=isClientSideResourse(request->resource);
request->mimeType=getMIMEType(request->resource);
continue;
}
//done done
if(request->data!=NULL)
{
for(int k=0;k<request->dataCount;k++)free(request->data[k]);
free(request->data);
}
break;
}
}
}// The infinite loop related to forwarding
}
} // the infinite loop related to accept method ends here
closesocket(serverSocketDescriptor);
WSACleanup();
return;

}

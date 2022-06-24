
#include<tmwp>
#include<iostream>
#include<string.h>
#include<fstream>
using namespace tmwp;
using namespace std;

class studentData
{
int rollNumber;
char name[52];
public : 
void setName(string name)
{
strcpy(this->name,name.c_str());
}

int getRollNumber()
{
return this->rollNumber;
}

void setRollNumber(int rollNumber)
{
this->rollNumber=rollNumber;
}

char* getName()
{
return this->name;
}
};

void addStud(Request &request,Response &response)
{
string a=request.get("nm");
string e=request.get("rollNumber");

studentData d;
int rollN=atoi(e.c_str());
d.setName(a);
d.setRollNumber(rollN);
cout<<a<<endl;
ofstream file("Student.dat",ios::app | ios::binary);
if(file.fail()) return;
file.write((char *)&d,sizeof(d));
file.close();
response.write("<!DOCTYPE HTML><html lang='en'><head><title>Adding Status</title></head><body><h1>Student Added </h1><br><br><br><br><a href='index.html'><h2>OK</h2></a><br></body></html>");
response.close();
}

void getAllStudents(Request& request,Response& response)
{
char s[1501];
char tmp[501];
studentData d;
int i=1;

strcpy(s,"<!DOCTYPE HTML><html lang='en'><head><title>List Of Students</title></head><body><table border='1'><tr><td>S. No.</td><td>Roll No.</td><td>Name</td></tr>");
ifstream f("Student.dat",ios::binary);
while(!f.fail())
{
f.read((char*)&d,sizeof(studentData));
if(f.fail()) break;
sprintf(tmp,"<tr><td>%d</td><td>%d</td><td>%s</td></tr>",i,d.getRollNumber(),d.getName());
strcat(s,tmp);
i++;
}
f.close();
strcat(s,"</table><br><a href='index.html'>OK</a><br></body></html>");
response.write(string(s));
response.close();
}

void sendStudentEditForm(Request &rq,Response &rs)
{
studentData d;
string e=rq.get("rollNumber");
int rl=atoi(e.c_str());
int found=0;
ifstream f("Student.dat",ios::binary);
int n;

while(!f.fail())
{
f.read((char *)&d,sizeof(studentData));
if(f.fail()) break;
n=d.getRollNumber();
if(n==rl)
{
found=1;
break;
}
}
f.close();
char tmp[1001];

if(found==0)
{
sprintf(tmp,"<!DOCTYPE HTML><html lang='en'><meta charset='utf-8'><head><title>Update Status</title></head><body><h1>Student having RollNumber /%d Not Found</h1></body></html>",rl);
rs.write(string(tmp));
}
else
{
sprintf(tmp,"<!DOCTYPE HTML><html lang='en'><meta charset='utf-8'><head><title>Update Status</title></head><body><form method='GET' action='updateStudent'>Name <input type='text' id='nm' name='nm' value='%s'> Roll Number <input type='text' id='rollNumber' name='rollNumber' value='%d'> <br><br><button type='Submit'>Update</button></form><a href='index.html'>Home</a></body></html>",d.getName(),rl,rl);
rs.write(string(tmp));
}
}

void updateStudentData(Request &rq,Response &rs)
{
studentData s;
studentData a;
a.setName(rq.get("nm"));
a.setRollNumber(atoi(rq.get("rollNumber").c_str()));

// update the student details from file handling

fstream f("Student.dat",ios::out | ios::in | ios::binary);
fstream tmpFile("tmp.dat",ios:: out | ios::binary);
int found=0;

while(1)
{
f.read((char *)&s,sizeof(studentData));
if(f.fail()) break;
if(s.getRollNumber()==a.getRollNumber())
{
found=1; 
tmpFile.write((char *)&a,sizeof(studentData));
}
else tmpFile.write((char *)&s,sizeof(studentData));
}

tmpFile.close();
f.close();

fstream tm("tmp.dat",ios::in | ios::binary);

fstream newFile("Student.dat",ios::in | ios::out | ios::binary);

studentData data;
while(1)
{
tm.read((char*)&data,sizeof(studentData));
if(tm.fail()) break;
newFile.write((char *)&data,sizeof(studentData));
}

newFile.close();
tm.close();

fstream ff("tmp.dat",ios::binary);
ff.close();

char t[1034];
sprintf(t,"<!DOCTYPE HTML><html lang='en'><meta charset='utf-8'><head><title>Update Status</title></head><body>(Student Update Module)<h1>Student having RollNumber : %d is updated.</h1><a href='index.html'>Home</a></body></html>",a.getRollNumber());
rs.write(string(t));
}

void sendStudentDeleteConfirmationForm(Request &rq,Response &rs)
{
studentData d;
string e=rq.get("rollNumber");
int rl=atoi(e.c_str());
int found=0;
ifstream f("Student.dat",ios::binary);
int n;

while(!f.fail())
{
f.read((char *)&d,sizeof(studentData));
if(f.fail()) break;
n=d.getRollNumber();
if(n==rl)
{
found=1;
break;
}
}
f.close();
char tmp[1001];

if(found==0)
{
sprintf(tmp,"<!DOCTYPE HTML><html lang='en'><meta charset='utf-8'><head><title>Delete Status</title></head><body><h1>Student having RollNumber /%d Not Found</h1></body></html>",rl);
rs.write(string(tmp));
}
else
{
sprintf(tmp,"<!DOCTYPE HTML><html lang='en'><meta charset='utf-8'><head><title>Delete Status</title></head><body><form method='GET' action='deleteStudent'>Roll Number <input type='text' id='rollNumber' name='rollNumber' value='%d'><br>Name : %s<br><button type='Submit'>Delete</button></form><a href='index.html'>Home</a></body></html>",rl,d.getName());
rs.write(string(tmp));
}
}

void deleteStudentData(Request &rq,Response &rs)
{
studentData s;
int pos=0;
studentData a;
a.setName(rq.get("nm"));
a.setRollNumber(atoi(rq.get("rollNumber").c_str()));
// update the student details from file handling
fstream f("Student.dat",ios::out | ios::in | ios::binary);
fstream tmpFile("tmp.dat",ios::out | ios::binary);
while(1)
{
f.read((char *)&s,sizeof(studentData));
if(f.fail()) break;
if(s.getRollNumber()==a.getRollNumber()) continue;
//s.setName(a.getName());
//tmpFile.write((char *)&s,sizeof(studentData));
else tmpFile.write((char *)&s,sizeof(studentData));
}
 tmpFile.close();
f.close();

fstream tm("tmp.dat",ios::in | ios::binary);
fstream newFile("Student.dat",ios::out | ios :: binary);
studentData data;
while(1)
{
tm.read((char*)&data,sizeof(studentData));
if(tm.fail()) break;
newFile.write((char *)&data,sizeof(studentData));
}
newFile.close();
tm.close();

fstream ff("tmp.dat",ios::binary);
ff.close();
char t[1034];
sprintf(t,"<!DOCTYPE HTML><html lang='en'><meta charset='utf-8'><head><title>Update Status</title></head><body>(Student Delete Module)<h1>Student %d Deleted</h1><a href='index.html'>Home</a></body></html>",a.getRollNumber());
rs.write(string(t));
}



int main()
{
TMWebProjector server(8080);
server.onRequest("/addStudent",addStud);
server.onRequest("/getStudents",getAllStudents);
server.onRequest("/editStudent",sendStudentEditForm);
server.onRequest("/updateStudent",updateStudentData);
server.onRequest("/confirmDeleteStudent",sendStudentDeleteConfirmationForm);
server.onRequest("/deleteStudent",deleteStudentData);
server.start();
return 0;
}

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
struct exttable
{
    char cextsym[20], extsym[20];
    unsigned int address,length;
}estab[500];

struct objectcode
{
    unsigned char code[15];
    int add;
}obcode[500];

struct refarr
{
    unsigned int arr[100];
}ra[100];

int OPnum;
void main()
{
    char temp[10];
    FILE *fp1,*fp2,*fp3;
    int i,j,x,y,pstart,exeloc,start,textloc,loc,textlen,length,location,st,s;
    int n=0,num=0,inc=0,count=0,record=0,mloc[30],mlen[30];
    signed long int newadd;
    char operation,lbl[10],input[10],label[50][10],opr[30],ch,*add1,address[10];
    int labelopnum[50];
    OPnum=0;
    fp1=fopen("linkinput2.txt","r");
    fp2=fopen("ESTAB.txt","r");
    fp3=fopen("OUTPUT.txt","w");
    while(!feof(fp2))
    {
        fscanf(fp2,"%s %s %X %X", estab[num].cextsym, estab[num].extsym, &estab[num].address,  
                                                &estab[num].length);
        num++;
    }
    exeloc=estab[0].address;
    loc=exeloc;
    start=loc;
    st=start;
    while(!feof(fp1))
    {
        fscanf(fp1,"%s",input);
        if(strcmp(input,"H")==0)
        {
            OPnum++;
            fscanf(fp1,"%s",input);
            for(i=0;i<num;i++)
                if(strcmp(input,estab[i].cextsym)==0)
                {
                    ra[OPnum].arr[1]=estab[i].address;
                    pstart=estab[i].address;
                    break;
                }
            while(strcmp(input,"R")!=0)
                fscanf(fp1,"%s",input);
        }
        char rname[10];
        if(strcmp(input,"R")==0)
        {
                fscanf(fp1,"%s",rname);
                while(strcmp(rname,"T")!=0)
                {
                    char rnum[10];
                    int rnumi;
                    rnum[0] = rname[0];
                    rnum[1] = rname[1];
                    rnum[2] = '\0';
                    rnumi = atoi(rnum);
                    strcpy(rname, &rname[2]);
                    for(i=0;i<num;i++)
                    {
                        if(strcmp(rname,estab[i].extsym)==0)
                        {
                            ra[OPnum].arr[rnumi]=estab[i].address;
                        }
                    }
                    fscanf(fp1,"%s",rname);   
                }
                strcpy(input,rname);
        }
        do
        {
            if(strcmp(input,"T")==0)
            {
                fscanf(fp1,"%x",&textloc);
                textloc=textloc+pstart;
                for(i=0;i<(textloc-loc);i++)
                {
                    strcpy(obcode[inc].code,"..");
                    obcode[inc++].add=start++;
                }
                fscanf(fp1,"%x",&textlen);
                loc=textloc+textlen;
            }
            else if(strcmp(input,"M")==0)
            {
                fscanf(fp1,"%x",&mloc[record]);
                mloc[record]=mloc[record]+pstart;
                fscanf(fp1,"%x",&mlen[record]);
                fscanf(fp1,"%s",label[record]);
                labelopnum[record++]=OPnum;
            }
            else
            {
                length=strlen(input);
                x=0;
                for(i=0;i<length;i++)
                {
                    obcode[inc].code[x++]=input[i];
                    if(x>1)
                    {
                        obcode[inc++].add=start++;
                        x=0;
                    }
                }
            }
            fscanf(fp1,"%s",input);
        }while(strcmp(input,"E")!=0);

        if(strcmp(input,"E")==0)
        fscanf(fp1,"%s",input);
    }
        for(n=0;n<record;n++)
        {
            int lbnumi;
            operation=label[n][0];
            char lbnum[10];
            lbnum[0]=label[n][1];
            lbnum[1]=label[n][2];
            lbnum[2]='\0';
            lbnumi=atoi(lbnum);
            length=0;
            strcpy(address,"\0");
            location=mloc[n]-exeloc;
            loc=location;
            count=0;
            while(length<mlen[n])
            {
                strcat(address,obcode[location++].code);
                count++;
                length+=2;
            }
            switch(operation)
            {
                case '+':
                        newadd=strtol(address,&add1,16)+(long int)ra[labelopnum[n]].arr[lbnumi];
                        break;
                case '-':
                        newadd=strtol(address,&add1,16)-(long int)ra[labelopnum[n]].arr[lbnumi];
                        break;
            }
            ltoa(newadd,address,16);
            x=0; y=0;
            while(count>0)
            {
                obcode[loc].code[x++]=address[y++];
                if(x>1)
                {
                        x=0; loc++;
                        count--;
                }
            }
    }
    count=0;
    n=0;
    s=st-16;
    fprintf(fp3,"%X\t",s);
    for(i=1;i<=16;i++)
    {
        fprintf(fp3,"xx");
        if(i==4||i==8||i==12)
        {
            fprintf(fp3,"\t");
        }
    }
    fprintf(fp3,"\n\n%lX\t",obcode[0].add);
    for(i=0;i<inc;i++)
    {
        fprintf(fp3,"%s",obcode[i].code);
        n++;
        if(n>3)
        {
            fprintf(fp3,"\t");
            n=0;
            count++;
        }
        if(count>3)
        {
            fprintf(fp3,"\n\n%lX\t",obcode[i+1].add);
            count=0;
        }
    }
    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
    printf("\n\t***** PASS TWO OF A DIRECT-LINKING LOADER *****\n");
    printf("\nThe contents of the output file :");
    printf("\n---------------------------------------------------------------");
    printf("\nAddress\t\t\t\tContents");
    printf("\n---------------------------------------------------------------\n");
    fp3=fopen("OUTPUT.txt","r");
    ch=fgetc(fp3);
    while(ch!=EOF)
    {
        printf("%c",ch);
        ch=fgetc(fp3);
    }
    fclose(fp3);
}
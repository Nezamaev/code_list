
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define N_pins 256

 
main(n_arg,arg)
  char   *arg[];    int    n_arg;
{  
   char s[512], *p, *p2, file[512];
   int  i,j,k;
   FILE *out, *in;

   struct T { char Oper[64]; char MSL[64];  char Dev_ID[32];  char PRG[64];   char Time[32]; } Title;
   struct par { char Name[64]; double Value; };
   struct Leakage {char Dev[64]; char Name[64];  char Temp[64];  int n; struct par Parms[N_pins]; } *newPar, *AllPar[512];
   int N=0;
   char Dev[64], sort[64], Temp[64];

   int nTemp = 0, nParm = 0;
   char Temps[N_pins][64], Parms[N_pins][64];
   int Data[N_pins][64];

     if ( n_arg < 2 )  
	   { printf(" *** ERROR Missing input file name\n",s); exit(0); }
     else  strcpy(file,arg[1]);

     if ((in = fopen(file,"r")) == 0)
	   { printf(" *** ERROR open for Read file '%s' \n",s); exit(0); }
		

	  fgets(s,512,in); s[strlen(s)-1]=0;
	  if (strncmp(s,"Operator:",9) == 0) { p=strchr(s,':')+2; strcpy(Title.Oper,p); }
	  else {printf("Bad TITLE  - No Operator in 1 line\n"); goto A; }
	  fgets(s,512,in); s[strlen(s)-1]=0;
	  if (strncmp(s,"MSL:",4) == 0) { p=strchr(s,':')+2; strcpy(Title.MSL,p); }
	  else {printf("Bad TITLE  - No MSL in 2 line\n"); goto A; }
	  fgets(s,512,in); s[strlen(s)-1]=0;
	  if (strncmp(s,"Device_ID:",10) == 0) { p=strchr(s,':')+2; strcpy(Title.Dev_ID,p);}
	  else {printf("Bad TITLE  - No Device_ID in 3 line\n"); goto A; }
	  fgets(s,512,in); s[strlen(s)-1]=0;
	  if (strncmp(s,"Test_Program:",13) == 0) { p=strchr(s,':')+2; strcpy(Title.PRG,p);}
	  else {printf("Bad TITLE  - No Test_Program in 4 line\n"); goto A; }
	  fgets(s,512,in); s[strlen(s)-1]=0;
	  if (strncmp(s,"Start_Time:",11) == 0) { p=strchr(s,':')+2; strcpy(Title.Time,p);}
	  else {printf("Bad TITLE  - No Start_Time in 5 line\n"); goto A; }

      strcpy(s,file); p=strrchr(s,'.'); *p = 0; strcat(s,".Stable"); strcpy(file,s);
	  if ((out = fopen(file,"w")) == 0)
	   { printf(" *** ERROR open for Write file '%s' \n",s); exit(0); }

D:	  while (!feof(in))   
		{ fgets(s,512,in); 
	      if (strncmp(s,"End_data",8)==0) 
			     break;
		  if (s[0] <=' ' || s[0] > '9') continue; 
          p=strchr(s, ' ');       
		  strncpy(Dev,s,p-s); Dev[p-s]=0;

          while (*p <= ' ') p++;    while (*p > ' ')  p++;   // 15
          while (*p <= ' ') p++;    while (*p > ' ')  p++;   // 15
          while (*p <= ' ') p++;    while (*p > ' ')  p++;   // PASS
          while (*p <= ' ') p++;    while (*p > ' ')  p++;   // Data
          while (*p <= ' ') p++;    while (*p > ' ')  p++;   // Time
          while (*p <= ' ') p++;
		  p2=p; while (*p2 > ' ')  p2++;  *p2=0;
          strcpy(Temp,p);
		  if (Temp[0] != '-') { strcpy(sort,"+");  strcat(sort,Temp); }
		  else strcpy(sort,Temp);
		  if (sort[2] == '.') { sprintf(Temp,"%c0%s",sort[0],sort+1); }
		  else strcpy(Temp,sort);
		  if (!nTemp) { strcpy(Temps[0],Temp); nTemp++; }
		  else { for (k=0; k < nTemp; k++) if (strcmp(Temps[k],Temp)==0) break;  
		         if (k == nTemp) { strcpy(Temps[k],Temp); nTemp++; }
		       }


C:        if (feof(in))
	         { printf("*** Error *** Unexpected EOF\n");  goto B;   }
		  fgets(s,512,in);  
          if (s[2] == ' ') goto C;  
		  if (strstr(s,"End_data_tf")) { 
		                                 goto D; }
		  if (!strstr(s,"leakage")) goto C;
		  if (s[0] != ' ' || s[1] != ' ' || s[2] == ' ')
	         { printf("*** Error *** Bad I parameter %d\n%s\n",N,s);  goto B;   }
          

		  newPar  = (struct Leakage *) calloc(1, sizeof(struct Leakage));
          if (newPar == NULL) 
	         { printf("*** Error *** No memory for newPar\n"); 
               goto B;
	         }
		  if (N == 512) 
	         { printf("*** Error *** N parameters > 512\n"); 
               goto B; 
	         }
          AllPar[N] = newPar;
		  p = strchr(&s[2],' ');
          strncpy(AllPar[N]->Name,&s[2],p-s-2);
		  if (!nParm) { strcpy(Parms[0],AllPar[N]->Name); nParm++; }
		  else { for (k=0; k < nParm; k++) 
			     if (strcmp(Parms[k],AllPar[N]->Name)==0) break;  
		         if (k == nParm) { strcpy(Parms[k],AllPar[N]->Name); nParm++; }
		       }

		  strcpy(AllPar[N]->Temp,Temp);
          strcpy(AllPar[N]->Dev,Dev);
          fgets(s,512,in);
		  if (!strstr(s,""))
	         { printf("*** Error *** Missing Data_format_pins:\n"); 
               goto B;
	         }
          fgets(s,512,in);
		  if (!strstr(s,""))
	         { printf("*** Error *** Missing Begin_data_pins:\n"); 
               goto B;
	         }
          i = 0;
          fgets(s,512,in);
          while (!strstr(s,"End_data_pins"))
		    {
	            printf("%d %s", i,s); 
				if (i == 128) // 32)
	              { printf("*** Error *** Too many pins %d:\n", i); 
                    goto B;
	               }  
				p=s;
                while (*p == ' ') p++;    
				p2 = p+1; 
				while ( *p2 > ' ')  
					p2++;   
	            strncpy(AllPar[N]->Parms[i].Name,p,p2-p);
				p=p2;
                while (*p == ' ') p++; 
				if (*p == 'N')
				 { while (*p > ' ')   p++;   // N 
				   while (*p <= ' ')  p++;   
				   while (*p > ' ')   p++;   // 0.000000 
				   while (*p <= ' ')  p++;   
				   while (*p > ' ')   p++;   // uA
				   while (*p <= ' ')  p++;   
				 }
				if (*p == 'P')
				  { while (*p > ' ')   p++;   // P
				    while (*p <= ' ')  p++;
					AllPar[N]->Parms[i].Value = atof(p);
				  }
				else
	              { printf("*** Error *** Missing P argument (%d)\n%s\n", i,s); 
                    goto B;
	               }
				i++;
				fgets(s,512,in);
		    }
          AllPar[N]->n = i;
		  N++;
          goto C;
		}

	fprintf(out,"    Operator: %s\n",Title.Oper);	
	fprintf(out,"         MSL: %s\n",Title.MSL);	
	fprintf(out,"   Device_ID: %s\n",Title.Dev_ID);	
	fprintf(out,"Test_Program: %s\n",Title.PRG);	
	fprintf(out,"  Start_Time: %s\n\n\n",Title.Time);	
/*
    for (i=0; i < N; i++)
	{ fprintf(out,"%s ",AllPar[i]->Dev);
	  fprintf(out,"%s\n",AllPar[i]->Temp);
	  fprintf(out,"   %s\n",AllPar[i]->Name);
	  for (k=0; k < AllPar[i]->n; k++)
	  	  fprintf(out,"         %s %f\n",AllPar[i]->Parms[k].Name, AllPar[i]->Parms[k].Value);
	  fprintf(out,"\n");
	}

    fprintf(out,"\n\n\n");
	for (i=0; i < nTemp; i++)
		 fprintf(out,"%s\n",Temps[i]);
    fprintf(out,"\n\n\n");
//	for (i=0; i < nParm; i++) fprintf(out,"%s\n",Parms[i]);


    fprintf(out,"\n\n\n");
*/
	for (i=0; i < nTemp-1; i++) 
	 { for (k=i+1; k < nTemp; k++) 
            if (atoi(Temps[i]) > atoi(Temps[k])) 
		          { strcpy(sort,Temps[k]); strcpy(Temps[k],Temps[i]); strcpy(Temps[i],sort); }
	  }
	
	for (i=0; i < nParm-1; i++) 
	 {  for (k=i+1; k < nParm; k++) 
		   if (strcmp(Parms[i],Parms[k]) < 0) 
		   { strcpy(sort,Parms[k]); strcpy(Parms[k],Parms[i]); strcpy(Parms[i],sort); }
	  }

	for (i=0; i < nTemp; i++) 
	{  for (k=0; k < nParm; k++)
	     { Data[i][k] = -1; 
	       for (j=0; j < N; j++)
	        if (!strcmp(Temps[i],AllPar[j]->Temp) && 
				!strcmp(Parms[k],AllPar[k]->Name))  
			  Data[i][k] = j; 
	      }
	}   

    for (j=0; j < N; j++)
	{  
	   for (k=0; k < nParm; k++) if (!strcmp(Parms[k],AllPar[j]->Name))  break;
	   for (i=0; i < nTemp; i++) if (!strcmp(Temps[i],AllPar[j]->Temp))  break;
       if ( k < nParm && i < nTemp) Data[i][k] = j;
	   else Data[i][k] = -1;
    }
/*
	   for (k=0; k < nParm; k++) 
	    { for (i=0; i < nTemp; i++)  
		     fprintf(out,"%d\t",Data[i][k]);
	      fprintf(out,"\n");
	    }


    fprintf(out,"\n\n\n");
	for (i=0; i < nTemp; i++)
		 fprintf(out,"%s\n",Temps[i]);
    fprintf(out,"\n\n\n");
	for (i=0; i < nParm; i++) fprintf(out,"%s\n",Parms[i]);


    fprintf(out,"\n\n\n");
*/

     for (k=0; k < nParm; k++)
	   { fprintf(out,"%s \t\t",Parms[k]);
		 for (i=0; i < nTemp; i++)  fprintf(out," %s\t\t",Temps[i]);
         fprintf(out," \n");
		 for (j=0; j < AllPar[Data[0][k]]->n; j++)
		  {  fprintf(out,"\t\t %s\t",AllPar[Data[0][k]]->Parms[j].Name);
			 for (i=0; i < nTemp; i++)  
			   { fprintf(out," %f\t",AllPar[Data[i][k]]->Parms[j].Value); }
/*
			   { if (AllPar[Data[i][k]]->Parms[j].Value > 0.20) fprintf(out," %f\t",AllPar[Data[i][k]]->Parms[j].Value); 
			     else fprintf(out," 0.200000\t");
			   }
*/
			 fprintf(out,"\n");
		  }
         fprintf(out," \n");
	}


	fclose(out);

	strcpy(s,file); p=strrchr(s,'.'); *p = 0; strcat(s,".Rtable"); strcpy(file,s);
	if ((out = fopen(file,"w")) == 0)
	   { printf(" *** ERROR open for Write file '%s' \n",s); exit(0); }

     for (k=0; k < nParm; k++)
	   { fprintf(out,"%s \t\t",Parms[k]);
		 for (i=1; i < nTemp; i++)  fprintf(out," %s\t\t",Temps[i]);
         fprintf(out," \n");
		 for (j=0; j < AllPar[Data[0][k]]->n; j++)
		  {  fprintf(out,"\t\t %s\t",AllPar[Data[0][k]]->Parms[j].Name);
			 for (i=1; i < nTemp; i++)     fprintf(out," %f\t",AllPar[Data[i][k]]->Parms[j].Value/AllPar[Data[i-1][k]]->Parms[j].Value); 
			 
			 fprintf(out,"\n");
		  }
         fprintf(out," \n");
	 }

	fclose(out);


    for (j=0; j < N; j++)
	  { for (k=0; k < AllPar[j]->n; k++)
	     { if (-0.15 < AllPar[j]->Parms[k].Value < 0.15) AllPar[j]->Parms[k].Value = 0.15; }
	  }


	strcpy(s,file); p=strrchr(s,'.'); *p = 0; strcat(s,".Atable"); strcpy(file,s);
	if ((out = fopen(file,"w")) == 0)
	   { printf(" *** ERROR open for Write file '%s' \n",s); exit(0); }

	fprintf(out,"    Operator: %s\n",Title.Oper);	
	fprintf(out,"         MSL: %s\n",Title.MSL);	
	fprintf(out,"   Device_ID: %s\n",Title.Dev_ID);	
	fprintf(out,"Test_Program: %s\n",Title.PRG);	
	fprintf(out,"  Start_Time: %s\n\n\n",Title.Time);	

     for (k=0; k < nParm; k++)
	   { fprintf(out,"%s \t\t\t\t",Parms[k]);
		 for (i=0; i < nTemp; i++)  fprintf(out," %s\t\t",Temps[i]);
         fprintf(out," \n");
		 for (j=0; j < AllPar[Data[0][k]]->n; j++)
		  {  fprintf(out,"\t\t %s\t",AllPar[Data[0][k]]->Parms[j].Name);
			 for (i=0; i < nTemp; i++)  
			   { fprintf(out," %f\t",AllPar[Data[i][k]]->Parms[j].Value);    }
			 fprintf(out,"\n");
		  }
         fprintf(out," \n");
	    }
	fclose(out);

	strcpy(s,file); p=strrchr(s,'.'); *p = 0; strcat(s,".Ktable"); strcpy(file,s);
	if ((out = fopen(file,"w")) == 0)
	   { printf(" *** ERROR open for Write file '%s' \n",s); exit(0); }

     for (k=0; k < nParm; k++)
	   { fprintf(out,"%s \t\t",Parms[k]);
		 for (i=1; i < nTemp; i++)  fprintf(out," %s\t\t",Temps[i]);
         fprintf(out," \n");
		 for (j=0; j < AllPar[Data[0][k]]->n; j++)
		  {  fprintf(out,"\t\t %s\t",AllPar[Data[0][k]]->Parms[j].Name);
			 for (i=1; i < nTemp; i++)      fprintf(out," %f\t",AllPar[Data[i][k]]->Parms[j].Value/AllPar[Data[i-1][k]]->Parms[j].Value); 
			 
			 fprintf(out,"\n");
		  }
         fprintf(out," \n");
	 }
B:	fclose(out);
A:	fclose(in);
	exit(0);
}

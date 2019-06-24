#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/fcntl.h>
#define LEN 10
#define ADD 3

struct l
{
  char *el;
  struct l *next;
};

typedef struct l wlst;

int issep (char c)
{
  return (c == '&' )||(c == '|')||(c == '>')||(c == '<');
}

char* getword(int mode, int *c, char *add)
{

	char *tmpw = (char*) malloc(LEN+1), *t;
	int len =LEN, i=0, c1;
	t=tmpw;

	if (mode) {c1= getchar();}
	else {c1=*c;};

	while (c1!=EOF && c1!= '"' && (mode || (!isspace(c1) && !issep(c1) && c1!='\n') )) //(mode || ((!isspace(c1))&&(!issep(c1)&&(c1!=EOF)&&(c1!='\n'))&& (c1!= '"'))))
  {
		if (i==len)
    {
			tmpw=(char*) realloc (tmpw,len+ADD+1);
			t= tmpw+len;
			len+=ADD;
		};
		*t=c1;
		t++;
		i++;
		c1= getchar();
    //printf("%c",c1);
	};
	*t='\0';

	if (add){ add= strcat (add,tmpw);}
	else {add=tmpw;};

	if (mode)
  {
		if (c1 =='"')
    {
			c1=getchar();
			*c=c1;
			if (isspace(c1)||(c1== EOF)) { return add;}
			else { return getword ( c1=='"', c, add);};
		}
		else
    {
      printf ("incorrect input \n");
			free (add);
			*c=' ';
			return NULL;
		 };
	 }
	 else
   { // not mode
		 *c=c1;
		 if (issep(c1)) {return add;}
		 else
     {
			if (c1=='"') {  return getword ( 1, c, add);}
			else {return add;}
    }
	 };
}



void addwlist(wlst **list, char* elem)
{
  if (!(*list))
  {// empty list
    wlst *tmp = (wlst*) malloc (sizeof(wlst));
    tmp->next =NULL;
    tmp->el = elem;
    (*list)= tmp;
  }
  else { addwlist( &((*list)->next), elem);};
  return;
}


void dellist( wlst *ls)
{
  if (ls)
  {
    wlst *tmp= ls;
    ls=ls->next;
    //printf("%s\n", tmp->el);
    free (tmp->el);
    free (tmp);
    dellist( ls);
  };
  return;
}

void addspsym (char c, wlst** list)
{
	char *tmp;
	tmp = (char*) malloc (2);
	*tmp = c;
	*(tmp+1)='\0';
	addwlist(list, tmp);
}

wlst* getlist(int *ch)
{
  wlst *tl=NULL;
  char *wrd;
  int c= getchar();
  while ((c!=EOF)&&(c!=('\n')))
  {

    while ( (c!=EOF)&&(c!=('\n'))&&(isspace(c)) )
    {
      c=getchar();
    };

    if ((c!='\n')&&(c!=EOF))
    {
      //it's a word
      wrd = getword (c=='"', &c, NULL);
      //printf("%s",wrd);
      // c has the next symbol after word
      if (wrd)
      {
  			if (strcmp(wrd, "")) {addwlist (&tl, wrd);}; //comes from dealing with "&"
  			if (issep(c))
        {
  				addspsym (c, &tl);
  				c=' ';
  			};
	    }
      else
      { // wrd = NULL => the stroke ended: \n or EOF
        dellist (tl);
        tl= NULL;
      };
    };
  };

  *ch = c;
  return tl;
};


void printlist( wlst* ls)
{
  if (ls)
  {
    printf("%s \n", ls->el);
    ls=ls->next;
    printlist (ls);
  };
  return;
}

void modifybrack(wlst *list)
{
  wlst *t,*list1=list;

  while ((list!=NULL) && (list->next!=NULL))
  {
    if ((strcmp(list->el,">")==0)&&(strcmp(list->next->el,">")==0))
    break;
    list=list->next;
  }
  //printf("%s\n",list->el);
  if ((list!=NULL)&&(list->next!=NULL))
  {
    free(list->el);
    list->el=malloc(3);
    list->el[0]='>';
    list->el[1]='>';
    list->el[2]='\0';
    t=list->next->next;
    free(list->next);
    list->next=t;

  }
  list=list1;
}

int checklist(wlst *list)
{

	int fnd1 =0 /*  &  */, fnd2 =0 /*  <  */, fnd3 =0 /* | */, fnd4 = 0 /* > */, fnd5 = 0 /* >> */, i=0;
    int ind1 = -1, ind2 = -1, ind3 = -1, ind4 = -1, ind5 = -1;
    modifybrack(list);
  //printlist(list);
	while (list!=NULL)
  {
		if (!strcmp(list->el, "&")) {fnd1=1; ind1 = i;}
		else
    {
      if (!strcmp(list->el, "<") ) {fnd2=1; ind2 = i;}
			else
      {
        if (!strcmp(list->el, "|"))
        {
            if (!fnd3) ind3 = i;
            fnd3 = 1;
        }
        if(!strcmp(list->el, ">")) {fnd4 = 1;ind4 = i;}
        if(!strcmp(list->el, ">>")) {fnd5 = 1;ind5 = i;}

      }
		}
		list= list->next; // next element
		i++;// number of the element
	}
    i--;

	/*if (fnd2)
  {
		if ( (!fnd1) && (!fnd3) && (list) && (!list->next) && (i!=1) && (!fnd4) && (!fnd5) )  return 2;
		else return -2;
	}
  else
  {
    if (fnd3>0)
    {
      if ((!fnd2) && (!fnd4) && (!fnd5)) return 3;
			else return -3;
    }
		else if (fnd1)
    {
			if (!list) return 1;
			else return -1;
		}
		else if (fnd4)
    {
      if ( (!fnd1) && (!fnd3) && (list) && (!list->next) && (i!=1) && (!fnd2) && (!fnd5) ) return 4;
      else return -4;
    }
    else if (fnd5)
    {
      if ( (!fnd1) && (!fnd3) && (list) && (!list->next) && (i!=1) && (!fnd2) && (!fnd5) ) return 5;
      else return -5;
    }
    else return 0;
  }*/
  if (fnd2)
  {
      //printf("%d %d %d", ind2, ind3, i);
    if ( (!fnd1) && (ind2 > 0) && ((ind2 + 1 == i) || ind2 + 2 == ind3) /*&& (!fnd4) && (!fnd5)*/ ){
        if (fnd3)return 3;
        else return 2;
    }
		else return -2;
  }
  if (fnd1)
  {
    if (ind1 == i) return 1;
    else return -1;
  }
  if (fnd4)
  {
    if ( (!fnd1) && (ind4 > 0) && (ind4 + 1 == i) && (!fnd2) && (!fnd5) )
    {
        if (fnd3)return 3;
        else return 4;
    }
    else return -4;
  }
  if (fnd5)
  {
    if ( (!fnd1) && (ind5 > 0) && (ind5 + 1 == i) && (!fnd2) && (!fnd4) )
    {
        if (fnd3)return 3;
        else return 5;
    }
    else return -5;
  }
  if (fnd3)
  {
      if (!fnd1) return 3;
      else return -3;
  }
  else return 0;
}



void printarr( char** a){
    if (a){
        char **tmp=a;
        while (*tmp){
            printf("%s \n", *tmp);
            tmp++;
        };
    };
    return;
}

int checkPIPE (wlst *list)
{
	if (!strcmp(list->el,"|")) return 0;
	else
  {
		int prev=0,n =0, ferr = 0;
		wlst *tmp = list->next;
		while ( (!ferr) && (tmp) )
    {
			if (!strcmp(tmp->el, "|"))
      {
				if (prev)  {ferr=1;}
				else {prev = 1; n++;}
			}
			else {prev = 0;}
			tmp=tmp->next;
		}
		if (ferr) return 0;
		else
    {
			if (prev) return 0;
			else return n;
		}
	}
}

wlst *bite (wlst **list)
{
	wlst *b=(*list), *b1 =b, *tmp = (*list)->next;
	while ( (tmp) && (strcmp(tmp->el,"|") != 0) )
  {
		b1->next= tmp;
		b1= tmp;
		tmp= tmp->next;
	}
	b1->next = NULL;
	if (tmp)
  {
		(*list) = tmp-> next;
		free (tmp);
	}
	else return NULL;
	return b;
}

char** trnsltLtoA(wlst *list)
{
  char **arr;
  if (list)
  {
    arr = (char**) malloc ((LEN+1)*(sizeof(char*)) );
    char **tarr=arr;
    int i=0;
    int len = LEN;
    wlst *tl=list;
    while (tl)
    {
      if (i==len)
      {
        arr= (char**) realloc(arr, (len+ADD+1)*(sizeof(char*)) );
        tarr=arr+len;
        len+=ADD;
      };
      *tarr=tl->el;
      tarr++;
      tl=tl->next;
      i++;
    };
    *tarr=NULL;
  } else { arr=NULL;};
  return arr;
}


void delarr(char** ar)
{
	char** tmp = ar;
	while (*tmp)
  {
		free(*tmp);
		tmp++;
	};
	free (ar);
  ar=NULL;
	return;
}

void modify1 (wlst *t)
{
	while (strcmp(((t->next)->el), "&") )
  {
	   t=t->next;
	}
	free ((t->next)->el);
	free (t->next);
	t->next=NULL;
}

char* modify2 (wlst* t)
{
	wlst *t1;
	char *f;
	t1=(t->next);
	while (t1 && strcmp((t1->el),"<") && strcmp((t1->el),">") && strcmp((t1->el),">>"))
    {
        t=t1;
		t1=t1->next;
	};
    if (t1)
    {
        f= t1->next->el;
        free(t1->next);
        free (t1);
        t->next=NULL;
        return f;
    } else return NULL;
}

int main()
{
	int st,n,c,fndkav;
	pid_t p,pidnorm,pd;
	char **array,*f;// buf[3];
	int fd, fd1[2], fd0,i;
  wlst *lst, *lst1;
	do {

		lst = getlist(&c);
		//printlist (lst);
		//printf ("%d \n", checklist(lst));
		switch (checklist(lst))
    {
      case 4:
      {//>

        f = modify2 (lst);

  			array = trnsltLtoA(lst);
        if (*array)
        {
  				if ((fd = open(f,O_TRUNC | O_CREAT | O_WRONLY,0666))==-1) { perror("file problems"); exit(4);};
  				switch (pd=fork())
          {
  					case -1: perror("!!!"); exit (3);
  					case 0:
            {
  						dup2 (fd,1);
  						close (fd);
  						execvp (*array, array);
  						perror ("! !"); exit (5);
  					}
  				}
  				close (fd);
  				waitpid (pd,&st,0);
  				if (array) {delarr(array);}
        }
      }
      break;
      case 5:
      {//>
        f = modify2 (lst);
  			array = trnsltLtoA(lst);
        if (*array)
        {
  				if ((fd = open(f,O_CREAT | O_WRONLY | O_APPEND,0666))==-1) { perror("file problems"); exit(4);};
  				switch (pd=fork())
          {
  					case -1: perror("!!!"); exit (3);
  					case 0:
            {
  						dup2 (fd,1);
  						close (fd);
  						execvp (*array, array);
  						perror ("! !"); exit (5);
  					}
  				}
  				close (fd);
  				waitpid (pd,&st,0);
  				if (array) {delarr(array);}
        }
      }
      break;
			case 0:
      {
				if (lst)
        {
  				array = trnsltLtoA(lst);
  				if (*array)
          {
  					if (strcmp( (*array),"cd") == 0)
            {
  						if (array[1])
              {
  							if (chdir (array[1]))
                {
  								printf ("no such file\n");};
  							}
  						else { chdir ( getenv("HOME"));}
  					}
  					else
            {
  						switch (pidnorm=fork())
              {
  							case -1: perror ("!"); exit (1); // break;
  							case 0: execvp(*array, array); perror("!!"); exit(2);
  						}
  						//wait (NULL);
  						//wait (&st);
  						waitpid (pidnorm,&st,0);
  						//printf ("normal mode term!");
  						if (WIFEXITED(st))
              {
  							printf ("for process in normal mode, status: %d\n", WEXITSTATUS(st));
  						}
  						else {printf("nothing to run -_- ");};
  					}
  					if (array) {delarr (array);}
  				};
				};
			}; break;

      case -1: printf("incorrect input, & is misplaced \n");
  		dellist (lst);
  		break;
      case -4: printf("incorrect input, > is misplaced \n");
  		dellist (lst);
  		break;
      case -5: printf("incorrect input, >> is misplaced \n");
  		dellist (lst);
  		break;
		  case -2: printf("incorrect input, something written after ... < progname\n");
		  dellist (lst);
			break;
	    case 3:
      {
  			if (!(n=checkPIPE (lst))) {printf("incorrect input, messed with pipe!");}
  			else
        {
          lst1 = bite(&lst);
          f = modify2(lst1);
          array = trnsltLtoA(lst1);
          pipe(fd1);
          switch (fork())
          {
          	case -1: exit(20);
          	case 0:
            {
                if (f != NULL)
                {
                    if ((fd = open(f,O_RDONLY,0666))==-1) { perror("file problems"); exit(4);};
                    dup2 (fd,0);
                    close (fd);
                }
          		dup2(fd1[1], 1);
          		close (fd1[1]);
          		close (fd1[0]);
          		execvp (*array, array);
          		exit (21);
          	}
          	default:
            {
          		//if (array) {delarr (array);}
          		close (fd1[1]);
          		fd0= fd1[0];
          		for (i=2; i<=n; i++)
              {
          			lst1 = bite(&lst);
          			array = trnsltLtoA(lst1);
          			pipe(fd1);
          			switch(fork())
                {
          				case -1: exit(22);
          				case 0:
                  {
          					dup2 (fd0, 0);
          					dup2 (fd1[1],1);
          					close (fd0);
          					close (fd1[0]);
          					close (fd1[1]);
          					execvp (*array, array);
          					exit (23);
          				}
          				default:
                  {
          					//delarr (array);
          					close (fd0);
          					fd0= fd1[0];
          					close (fd1[1]);
          				}
          			}
          		}
          		// last process
          		//if (array) {delarr(array);}
              fndkav=checklist(lst);
              f=modify2(lst);
          		array = trnsltLtoA(lst);

          		switch (fork())
              {

          			case -1: exit(24);
          			case 0:
                {
                  if(f!=NULL)
                  {
                    //printf("1\n" );
                    if (fndkav==4)
                    {
                    if ((fd = open(f,O_TRUNC | O_CREAT | O_WRONLY,0666))==-1) { perror("file problems"); exit(4);};
                    }
                    else
                    {
                      if ((fd = open(f,O_CREAT | O_WRONLY | O_APPEND,0666))==-1) { perror("file problems"); exit(4);};
                    }
            				dup2 (fd,1);
            				close (fd);

            			}
          				dup2(fd0,0);
          				close (fd0);
          				execvp (*array, array);
          				exit (25);
          			}
          			default:
                {
          				//if (array) {delarr (array);}
          				dellist (lst);
          				close (fd0);
          				for (i=0; i<n+1; i++)
                  {
          					wait (NULL);
          				}
          			}
          		}
          	}
          }
        }
		  }
		  break;
	    case -3: printf("incorrect input, messed with pipe");
	    break;
		  case 1:
      {
  			 if (strcmp((lst->el),"&") )
         {
    				//printf ("%d", getpid());
    				modify1 (lst);
    				// run in background mode

    				array = trnsltLtoA(lst);
    				if (*array)
            {
    					switch (fork())
              {
    						case -1: perror ("!!"); exit (2); // break;
    						case 0: execvp(*array, array); perror("!!!"); exit(3);
    					}
    				}
    				if (array) {delarr (array);}
  			  }
  				else
          {// only "&" was entered
  					dellist(lst);
  				}
			}
      break;
		case 2:
    {
			f = modify2 (lst);
			array = trnsltLtoA(lst);

			//ready to go <
			if (*array)
      {
				if ((fd = open(f,O_RDONLY,0666))==-1) { perror("file problems"); exit(4);};
				switch (pd=fork())
        {
					case -1: perror("!!!"); exit (3);
					case 0:
          {
						dup2 (fd,0);
						close (fd);
						execvp (*array, array);
						perror ("! !"); exit (5);
					}
				}
				close (fd);
				waitpid (pd,&st,0);
				if (array) {delarr(array);}
			} break;
		}
		};// the end of SWITCH
		while ((p=waitpid(-1,&st, WNOHANG))>0)
    {
			//p=waitpid(-1,&st, WNOHANG);
			if (p>0)
      {
        printf ("one of the child processes terminated (#%d) \n", p);
				if (WIFEXITED(st))
        {
					printf ("for process in background mode, status: %d\n", WEXITSTATUS(st));
				}
				else {printf("nothing to run");};
			}
		};
	 // nothing happened)

	} while (c!=EOF);
	return 0;
}

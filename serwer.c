#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>


#define LICZBAUZYTKOWNIKOW 9
#define GRUPY 3
#define MAX 512

struct kolejka{
        long type;
        char wiadomosc[512];
    };
    
struct kolejka2{
        long type;
        int value;
    };
    
struct message{
        long type;
        char wiadomosc[512];
    };
    
char *my_itoa(int num, char *str){
        if(str == NULL)
        {
                return NULL;
        }
        sprintf(str, "%d", num);
        return str;
} //int to string
void wczytajZPlikuKonfiguracyjnego(char uzytkownicy[9][6], char grupy[3][7]){
    int fd = open("plikKonfiguracyjny.txt",O_RDONLY);
    
    lseek(fd,13,SEEK_SET); //omijamy linie "Uzytkownicy: \n"
    int i;
    for(i=0;i<LICZBAUZYTKOWNIKOW;i++){
        char slowo[6];
        read(fd,slowo,5);
        strcpy(uzytkownicy[i],slowo);
        lseek(fd,1,SEEK_CUR);
       
        }
    lseek(fd,7,SEEK_CUR); //omijamy linie "Grupy: \n"
    for(i=0;i<GRUPY;i++){
        char slowo1[7];
        read(fd,slowo1,6);
        strcpy(grupy[i],slowo1);
        lseek(fd,1,SEEK_CUR);   //omijamy znak nowej linii
    }
    close(fd);
}

int main(){
    int i,j, idKolejki, idKolejki2, idMsg, temp, temp2;
    char uzytkownicy[LICZBAUZYTKOWNIKOW][6];
    char grupy[GRUPY][7];
    int grupa0[9],grupa1[9],grupa2[9];
    int zalogowani[LICZBAUZYTKOWNIKOW];
    struct kolejka kolej;
    struct kolejka2 kolej2;
    struct message msg;
    wczytajZPlikuKonfiguracyjnego(uzytkownicy,grupy);
    
    
    for(i=0;i<LICZBAUZYTKOWNIKOW;i++){
         zalogowani[i]=0;
         grupa0[i]=-1;
         grupa1[i]=-1;
         grupa2[i]=-1;
     }
     
    idKolejki = msgget(123123,IPC_CREAT|IPC_EXCL|0600);
    if (idKolejki == -1){
        idKolejki = msgget(123123, IPC_CREAT|0600);
        if (idKolejki == -1){
        perror("Utworzenie kolejki komunikatow");
        }
    }
    idKolejki2 = msgget(222444,IPC_CREAT|IPC_EXCL|0600);
     if (idKolejki2 == -1){
        idKolejki2 = msgget(222444, IPC_CREAT|0600);
        if (idKolejki2 == -1){
        perror("Utworzenie kolejki komunikatow");
        }
    }
    idMsg = msgget(99700,IPC_CREAT|IPC_EXCL|0600);
     if (idMsg == -1){
        idMsg = msgget(99700, IPC_CREAT|0600);
        if (idMsg == -1){
        perror("Utworzenie kolejki komunikatow");
        }
    }
    
    msgctl(idKolejki,IPC_RMID,0);
    msgctl(idKolejki2,IPC_RMID,0);
    msgctl(idMsg,IPC_RMID,0);
    
    idKolejki = msgget(123123,IPC_CREAT|IPC_EXCL|0600);
    if (idKolejki == -1){
        idKolejki = msgget(123123, IPC_CREAT|0600);
        if (idKolejki == -1){
        perror("Utworzenie kolejki komunikatow");
        }
    }
    idKolejki2 = msgget(222444,IPC_CREAT|IPC_EXCL|0600);
     if (idKolejki2 == -1){
        idKolejki2 = msgget(222444, IPC_CREAT|0600);
        if (idKolejki2 == -1){
        perror("Utworzenie kolejki komunikatow");
        }
    }
    idMsg = msgget(99700,IPC_CREAT|IPC_EXCL|0600);
     if (idMsg == -1){
        idMsg = msgget(99700, IPC_CREAT|0600);
        if (idMsg == -1){
        perror("Utworzenie kolejki komunikatow");
        }
    }
    
    
    
    for(i=0;i<LICZBAUZYTKOWNIKOW;i++){
        printf("%s\n",uzytkownicy[i]);
    }
    for(i=0;i<GRUPY;i++){
        printf("%s\n", grupy[i]);
    }
    while(1){
    msgrcv(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),0,0);
    if(kolej.type==1){
         bool x = false;
         printf("%s\n",kolej.wiadomosc);
         
        for(i=0;i<LICZBAUZYTKOWNIKOW;i++){ //przegladamy tablice z nazwami uzytkownikow
        if(strcmp(kolej.wiadomosc,uzytkownicy[i])==0){//jesli wprwoadzony login sie zgadza z jedna z nazw uzytkownikow
        printf("true\n");
        kolej2.type=1;
        kolej2.value= i+1;
        msgsnd(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),0); //odsylamy uzytkownikowi jego id
        
        x = true;
        zalogowani[i]=1; //zaznaczamy ze uzytkownik sie zalogowal
        printf("%s zalogowano \n",kolej.wiadomosc);
        
        break;
         }
        }
        
        if(!x){ //jesli nie ma takiego loginu
        printf("false\n");
        kolej2.value= -1;
        kolej2.type = 1;
        msgsnd(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),0); //wysylamy ze nie udalo sie zalogowac 
        }
    }//logowanie
    if(kolej.type==2){
        int licznik=0;
         for(i=0;i<LICZBAUZYTKOWNIKOW;i++){
             if(zalogowani[i]==1){
                licznik++;  //liczymy zalogowanych uzytkownikow
             }
         }
         kolej2.value = licznik;
         kolej2.type=2;
         
         msgsnd(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),0); // wysylamy liczbe zalogowanych
     
         msgrcv(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),2,0); //otrzymujemy "ok" jesli uzytkownik odebral liczbe
         if(strcmp(kolej.wiadomosc,"ok")==0){
         for(i=0;i<LICZBAUZYTKOWNIKOW;i++){//w petli wysylamy id zalogowanych
             if(zalogowani[i]==1){
                kolej2.value=i+1;
                msgsnd(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),0);
             }
         }
         
         }
    }//zalogowani uzytkownicy
    if(kolej.type==3){
        int temp = atoi(kolej.wiadomosc); //id wylogowywujacego sie uzytkownika
        zalogowani[temp-1]=0; //zaznaczamy ze sie wylogowal
        for(i=0;i<LICZBAUZYTKOWNIKOW;i++){ //wypisujemy go z grup jesli do jakichs nalezal
            if(grupa0[i]==temp){
                grupa0[i]=-1;
            }
            if(grupa1[i]==temp){
                grupa1[i]=-1;
            }
            if(grupa2[i]==temp){
                grupa2[i]=-1;
            }
        }
    }//wylogowanie uzytkownika
    if(kolej.type==4){
        int licznik=0;
         
         kolej2.value = GRUPY;
         kolej2.type=4;
         
         msgsnd(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),0); //na poczatek wysylamy liczbe grup
     
         msgrcv(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),4,0);//odbieramy "ok" jesli uzytkownik odebral liczbe grup
         if(strcmp(kolej.wiadomosc,"ok")==0){
             
         int choosing = 0; //aktualna przetwarzana grupa
         for(i=0;i<GRUPY;i++){  //w petli wysylamy liczbe uzytkownikow w kazdej grupie  
         for(j=0;j<LICZBAUZYTKOWNIKOW;j++){
             if(choosing==0){
             if(grupa0[j]!=-1){
                 licznik++;
             }
             }else if(choosing == 1){
               if(grupa1[j]!=-1){
                 licznik++;
             }  
             }else if(choosing == 2){
                if(grupa2[j]!=-1){
                 licznik++;
             } 
             }
         }
         kolej2.value = licznik;
         kolej2.type=4;
         msgsnd(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),0);
         choosing++;
         licznik=0;
         }
         }
         licznik=0;
         //int choosing=0;
         msgrcv(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),4,0); //odbieramy nr grupy do ktorej chce sie zapisac uzytk.
         temp = atoi(kolej.wiadomosc) - 1;
         
         kolej2.value=1;
         kolej2.type=4;
         msgsnd(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),0);//wysylamy uzytkownikowi "1" ze odebralismy zadanie
         msgrcv(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),4,0);//otrzymujemy id uzytkownika
         int temp2 = atoi(kolej.wiadomosc);
        
         kolej2.value=1;
         for(i=0;i<LICZBAUZYTKOWNIKOW;i++){//zapisujemy uzytkownika do grupy, chyba ze jest do niej zapisany
             if(temp==0){
                if(grupa0[i]==temp2){
                    kolej2.value=-1;
                    break;
                }
                if(grupa0[i]==-1){
                    grupa0[i]=temp2;
                    break;
                } 
             }else if(temp==1){
                 if(grupa1[i]==temp2){
                    kolej2.value=-1;
                    break;
                }
                 if(grupa1[i]==-1){
                    grupa1[i]=temp2;
                    break;
                }
             }else if(temp ==2){
                 if(grupa2[i]==temp2){
                    kolej2.value=-1;
                    break;
                }
                 if(grupa2[i]==-1){
                    grupa2[i]=temp2;
                    break;
                }
             }
         }
         kolej2.type=4;
         msgsnd(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),0);//wysylamy "1" lub "-1" jesli sie udalo lub nie udalo
    }//zapisanie sie do grupy
    if(kolej.type==5){
        int licznik=0;
         
         kolej2.value = GRUPY;
         kolej2.type=5;
         
         msgsnd(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),0); //na poczatek wysylamy liczbe grup
     
         msgrcv(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),5,0);//otrzymujemy "ok" jesli uzytk. otrzymal liczbe grup
         if(strcmp(kolej.wiadomosc,"ok")==0){
             
         int choosing = 0;//aktualnie przetwarzana grupa
         for(i=0;i<GRUPY;i++){    //w petli wysylamy ilosc uzytkownikow kazdej grupy
         for(j=0;j<LICZBAUZYTKOWNIKOW;j++){
             if(choosing==0){
             if(grupa0[j]!=-1){
                 licznik++;
             }
             }else if(choosing == 1){
               if(grupa1[j]!=-1){
                 licznik++;
             }  
             }else if(choosing == 2){
                if(grupa2[j]!=-1){
                 licznik++;
             } 
             }
         }
         kolej2.value = licznik;
         kolej2.type=5;
         msgsnd(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),0);
         choosing++;
         licznik=0;
         }
         }
         msgrcv(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),5,0); //otrzymujemy wybor grupy
         temp = atoi(kolej.wiadomosc) - 1;
         for(i=0;i<LICZBAUZYTKOWNIKOW;i++){// liczymy ilosc uzytkownikow wybranej grupy
             if(temp==0){
                 if(grupa0[i]!=-1){
                     licznik++;
                 }
             }
             if(temp==1){
                 if(grupa1[i]!=-1){
                     licznik++;
                 }
             }
             if(temp==2){
                 if(grupa2[i]!=-1){
                     licznik++;
                 }
             }
         }
         kolej2.value=licznik;
         kolej2.type=5;
         msgsnd(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),0);//wysylamy ilosc uzytk. wybranej grupy
         msgrcv(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),5,0);//otrzymujemy "ok" jesli uzytk. odebral liczbe
         if(strcmp(kolej.wiadomosc,"ok")==0){
         for(i=0;i<licznik;i++){//w petli wysylamy id uzytkownikow zapisanych do wybranej grupy
             if(temp==0){
                kolej2.value=grupa0[i];
                kolej2.type=5;
                msgsnd(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),0);
             }
             if(temp==1){
                kolej2.value=grupa1[i];
                kolej2.type=5;
                msgsnd(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),0);
             }
             if(temp==2){
                kolej2.value=grupa2[i];
                kolej2.type=5;
                msgsnd(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),0);
             }
         }
         
         }
         licznik=0;
    }//wyswietlenie uzytkownikow danej grupy
    if(kolej.type==6){
        temp = atoi(kolej.wiadomosc); //nr grupy z ktorej chce sie wypisac uzytkownik
        printf("nr grupy %d\n",temp);
        kolej2.value=1;
        kolej2.type=6;
        msgsnd(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),0);//odsylamy "1" ze odebralismy zadanie
        msgrcv(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),6,0);//otrzymujemy id uzytkownika
        temp2 = atoi(kolej.wiadomosc); //id uzytkownika
        printf("id %d\n",temp2);
        for(i=0;i<LICZBAUZYTKOWNIKOW;i++){//wypisujemy go z grupy
            if(temp==0){
                if(grupa0[i]==temp2){
                    grupa0[i]=-1;
                    printf("true\n");
                    break;
                }
            }
            if(temp==1){
                if(grupa1[i]==temp2){
                    grupa1[i]=-1;
                    printf("true\n");
                    break;
                }
            }
            if(temp==2){
                if(grupa2[i]==temp2){
                    grupa2[i]=-1;
                    printf("true\n");
                    break;
                }
            }
        }
    }//wypisywanie sie z grupy
    if(kolej.type==7){
        int idNadawcy = atoi(kolej.wiadomosc); //otrzymujemy id nadawcy
        printf("id nadawcy: %d\n",idNadawcy);
        int licznik=0;
        int counter=0;
        int zalogowaniTemp[LICZBAUZYTKOWNIKOW];
        for(i=0;i<LICZBAUZYTKOWNIKOW;i++){
            zalogowaniTemp[i]=-1;
        }
         for(i=0;i<LICZBAUZYTKOWNIKOW;i++){
             if(zalogowani[i]==1){
                licznik++; 
             }
         }
         kolej2.value = licznik;
         kolej2.type=7;
         
         msgsnd(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),0); //na poczatek wysylamy liczbe zalogowanych
     
         msgrcv(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),7,0);//otrzymujemy "ok" jesli user przyjal liczbe
         if(strcmp(kolej.wiadomosc,"ok")==0){
         for(i=0;i<LICZBAUZYTKOWNIKOW;i++){//wysylamy id zalogowanych
             if(zalogowani[i]==1){
                zalogowaniTemp[counter]=i+1;
                counter++;
                kolej2.value=i+1;
                msgsnd(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),0);
             }
         }
         msgrcv(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),7,0);//otrzymujemy id odbiorcy
         int idOdbiorcy = zalogowaniTemp[atoi(kolej.wiadomosc)];
         printf("id odbiorcy: %d\n",idOdbiorcy);
         kolej2.value=1;
         msgsnd(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),0);//wysylamy "1" ze przyjelismy zadanie
         msgrcv(idMsg,&msg,sizeof(struct message)-sizeof(long),10,0);//otrzymujemy tresc wiadomosci
         char wiad[512];
         strcpy(wiad,msg.wiadomosc);
         msg.type=idOdbiorcy; 
         strcpy(msg.wiadomosc,wiad);
         msgsnd(idMsg,&msg,sizeof(struct message)-sizeof(long),0);//wysylamy wiadomosc do odbiorcy
         msgrcv(idMsg,&msg,sizeof(struct message)-sizeof(long),idOdbiorcy,0); //otrzymujemy "ok" jesli odbiorca przyjal wiadomosc
         if(strcmp(msg.wiadomosc,"ok")==0){
             my_itoa(idNadawcy,msg.wiadomosc);
             msg.type=idOdbiorcy;
             msgsnd(idMsg,&msg,sizeof(struct message)-sizeof(long),0);//wysylamy odbiorcy id nadawcy
         }
         }
    }//wysylanie wiadomosci do uzytkownika
    if(kolej.type==8){
        char wiadom[512];
        temp = atoi(kolej.wiadomosc); //otrzymujemy nr grupy do ktorej nadac wiadomosc
        printf("nr grupy %d\n",temp);
        kolej2.value=1;
        kolej2.type=8;
        msgsnd(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),0);//wysylamy "1" ze przyjelismy nr grupy
        msgrcv(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),8,0);//otrzymujemy id nadawcy
        int idNadawcy = atoi(kolej.wiadomosc); //id uzytkownika nadajacego wiadomosc
        printf("id %d\n",idNadawcy);
        kolej2.value=1;
        kolej2.type=8;
        msgsnd(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),0);//wysylamy "1" ze przyjelismy id
        msgrcv(idMsg,&msg,sizeof(struct message)-sizeof(long),10,0);//otrzymujemy tresc wiadomosci
        strcpy(wiadom,msg.wiadomosc);
        for(i=0;i<LICZBAUZYTKOWNIKOW;i++){//przegladamy uzytkownikow
            if(temp==0){//jesli wybrana byla grupa 0
                if(grupa0[i]!=-1 && grupa0[i]!=idNadawcy){ //jesli uzytkownik jest w grupie i nie jest nami
                printf("id odbiorcy %d\n",grupa0[i]);
                msg.type=grupa0[i];
                strcpy(msg.wiadomosc,wiadom);
                msgsnd(idMsg,&msg,sizeof(struct message)-sizeof(long),0);//wysylamy mu tresc
                msgrcv(idMsg,&msg,sizeof(struct message)-sizeof(long),grupa0[i],0);//otrzymujemy "ok" jesli otrzymal wiadomosc
                if(strcmp(msg.wiadomosc,"ok")==0){
                    my_itoa(idNadawcy,msg.wiadomosc);
                    msg.type=grupa0[i];
                    msgsnd(idMsg,&msg,sizeof(struct message)-sizeof(long),0);//wysylamy mu id nadawcy
                     } 
                }
            }
            if(temp==1){//jesli wybrana byla grupa1
                if(grupa1[i]!=-1 && grupa1[i]!=idNadawcy){
                printf("id odbiorcy %d\n",grupa1[i]);
                msg.type=grupa1[i];
                strcpy(msg.wiadomosc,wiadom);
                msgsnd(idMsg,&msg,sizeof(struct message)-sizeof(long),0);
                msgrcv(idMsg,&msg,sizeof(struct message)-sizeof(long),grupa1[i],0);
                if(strcmp(msg.wiadomosc,"ok")==0){
                    my_itoa(idNadawcy,msg.wiadomosc);
                    msg.type=grupa1[i];
                    msgsnd(idMsg,&msg,sizeof(struct message)-sizeof(long),0);
                     } 
                }
            }
            if(temp==2){//jesli wybrana byla grupa2
                if(grupa2[i]!=-1 && grupa2[i]!=idNadawcy){
                printf("id odbiorcy %d\n",grupa2[i]);
                msg.type=grupa2[i];
                strcpy(msg.wiadomosc,wiadom);
                msgsnd(idMsg,&msg,sizeof(struct message)-sizeof(long),0);
                msgrcv(idMsg,&msg,sizeof(struct message)-sizeof(long),grupa2[i],0);
                if(strcmp(msg.wiadomosc,"ok")==0){
                    my_itoa(idNadawcy,msg.wiadomosc);
                    msg.type=grupa2[i];
                    msgsnd(idMsg,&msg,sizeof(struct message)-sizeof(long),0);
                     } 
                }
            }
            }
        }//wysylanie wiadomosci do grupy
    }

    
    
    
}
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/sem.h>


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
#define LICZBAUZYTKOWNIKOW 9
#define GRUPY 3
#define MAX 512
char nazwa[10];
struct kolejka kolej;
struct kolejka2 kolej2;
struct message msg;

char *my_itoa(int num, char *str){
        if(str == NULL)
        {
                return NULL;
        }
        sprintf(str, "%d", num);
        return str;
} //int to string

bool wlaczenie(int idKolejki,int idKolejki2, int *id){
    int wybor;
    
    printf("witaj na czacie! \n");
    printf("wybierz co chcesz zrobic: \n");
    printf("1. Logowanie\n2. Wyjscie\n");
    scanf("%d",&wybor);
    switch(wybor){
        case 1:
          
         printf("wprowadz nazwe uzytkownika:\n");
         scanf("%s", nazwa);
         kolej.type = 1;
         strcpy(kolej.wiadomosc,nazwa);
         
         
         msgsnd(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),0);//wysylamy nasz login
       
         msgrcv(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),1,0); //odbieramy id albo "-1" jesli nie udalo sie zalogowac
         *id = kolej2.value;
            if(kolej2.value > 0){ //jesli dostalismy wartosc wieksza od 0 tzn ze dostalismy nasze id tzn ze sie udalo
                 printf("pomyslnie zalogowano \n");
                 return true;
             }else if(kolej2.value==-1){ //jesli -1 tzn ze zly login
                 printf("nieudane logowanie :( \n");
                 return false;
            }
               
            break;
        case 2:
        exit(0);
        break;
    }
    return false;
}

    
int main(){
   int i,id, wyborGrupy, temp, licznik; 
   int mojeGrupy[GRUPY];
   int posiadaneGrupy[GRUPY+1]; //pomocnicze
   char wiad[512];
   char wiadom[512];
   for(i=0;i<GRUPY;i++){
       mojeGrupy[i]=0; //na poczatku nie jestesmy zapisani do zadnej z grup
   }
   int idKolejki = msgget(123123,IPC_CREAT|IPC_EXCL|0600);
    if (idKolejki == -1){
        idKolejki = msgget(123123, IPC_CREAT|0600);
        if (idKolejki == -1){
        perror("Utworzenie kolejki komunikatow");
        }
    }
    int idKolejki2 = msgget(222444,IPC_CREAT|IPC_EXCL|0600);
     if (idKolejki2 == -1){
        idKolejki2 = msgget(222444, IPC_CREAT|0600);
        if (idKolejki2 == -1){
        perror("Utworzenie kolejki komunikatow");
        }
    } 
    int idMsg = msgget(99700,IPC_CREAT|IPC_EXCL|0600);
     if (idMsg == -1){
        idMsg = msgget(99700, IPC_CREAT|0600);
        if (idMsg == -1){
        perror("Utworzenie kolejki komunikatow");
        }
    }
    int *wsk = &id;
    if(wlaczenie(idKolejki,idKolejki2,wsk)){
    
    if(fork()==0){
        while(1){
        char temp[512];
        msgrcv(idMsg,&msg,sizeof(struct message)-sizeof(long),id,0); //odbieramy wiadomosc
        if(strcmp("koniec,prosze wyloguj sie procesie odbierajacy wiadomosci",msg.wiadomosc)==0){
            exit(0);
        }
        strcpy(temp,msg.wiadomosc); //zapisujemy ja w zmiennej temp
        strcpy(msg.wiadomosc,"ok");
        
        msg.type=id;
        msgsnd(idMsg,&msg,sizeof(struct message)-sizeof(long),0);//wysylamy wiadomosc "ok", mowiaca ze odebralismy wiadomosc
        
        msgrcv(idMsg,&msg,sizeof(struct message)-sizeof(long),id,0);//odbieramy id nadawcy
        int x=atoi(msg.wiadomosc);
        printf("\n");
        printf("Wiadomosc od uzytkownika test%d:\n",x);
        printf("%s",temp); //drukujemy wiadomosc od uzytkownika
        }
    }else{
    
    while(1){
        printf("Uzytkownik: test%d\n",id);
        printf("Co bys chcial zrobic:\n");
        printf("1.zobacz zalogowanych uzytkownikow\n");
        printf("2.zobacz zapisanych do danej grupy\n");
        printf("3.zapisz sie do grupy\n");
        printf("4.wypisz sie z grupy\n");
        printf("5.wyslij wiadomosc do uzytkownika\n");
        printf("6.wyslij wiadomosc do grupy\n");
        printf("7.wyloguj sie\n");
        int wybor,i;
        
        scanf("%d",&wybor);
        switch(wybor){
            case 1: //wyswietlanie zalogowanych uzytkownikow
                printf("Zalogowani: \n");
                kolej.type =2;
                msgsnd(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),0);//prosba o zalogowanych uzytkownikow
                
                msgrcv(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),2,0); //otrzymujemy liczbe zalogowanych uzytk.
            
                int liczba = kolej2.value;
               
                strcpy(kolej.wiadomosc,"ok");
                msgsnd(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),0);//wysylamy "ok" ze odebralismy liczbe
               
                for(i=0;i<liczba;i++){ //w petli otrzymujemy id zalogowanych i ich wypisujemy
                msgrcv(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),2,0);
                printf("test%d\n",(kolej2.value));
                }
                printf("\n");
                break;
            case 2://wyswietlanie uzytkownikow zapisanych do danej grupy
             printf("Wyswietl uzytkownikow zapisanych do danej grupy. Dostepne grupy: \n");
                kolej.type = 5;
                msgsnd(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),0);//zadanie o wyswietlenie uzytkownikow grupy
                
                msgrcv(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),5,0);//odbieramy liczbe grup
            
                liczba = kolej2.value;
               
                strcpy(kolej.wiadomosc,"ok");
                msgsnd(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),0);//wysylamy "ok" na potwierdzenie ze otrymalismy l. grup
               
                for(i=0;i<GRUPY;i++){//w petli otrszymujemy ilosc uzytk. danej grupy i wypisujemy
                msgrcv(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),5,0);
                printf("%d. grupa%d - %d uzytkownikow\n",(i+1),i,(kolej2.value));
                }
                printf("\n");
                
                printf("Wybierz grupe: ");
                scanf("%d",&wyborGrupy); //wybieramy grupe
                my_itoa(wyborGrupy,kolej.wiadomosc);
                kolej.type = 5;
                msgsnd(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),0);//wysylamy nasz wybor
                msgrcv(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),5,0);//otrzymujemy liczbe uzytk. wybranej grupy
                temp = kolej2.value;
                strcpy(kolej.wiadomosc,"ok");
                msgsnd(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),0);//wysylamy "ok" ze otrzymalismy liczbe
                for(i=0;i<temp;i++){ //w petli otrzymujemy id uzytkownikow zapisanych do danej grupy i wypisujemy ich
                  msgrcv(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),5,0);
                  printf("test%d\n",(kolej2.value));
                }
                
                printf("\n");
                break;
            case 3://zapisywanie do grupy
             printf("Dostepne grupy: \n");
                kolej.type =4;
                msgsnd(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),0); //wysylamy ze chcemy sie dopisac do grupy
                
                msgrcv(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),4,0); //otrzymujemy liczbe grup
            
                liczba = kolej2.value;
               
                strcpy(kolej.wiadomosc,"ok");
                msgsnd(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),0);//wysylamy "ok" ze otrzymalismy liczbe grup
               
                for(i=0;i<GRUPY;i++){//orzymujemy liczbe uzytkownikow danej grupy i wypisujemy grupaX - Y uzytkownikow
                msgrcv(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),4,0);
                printf("%d. grupa%d - %d uzytkownikow\n",(i+1),i,(kolej2.value));
                }
                printf("\n");
                int wyborGrupy;
                printf("Wybierz grupe: ");
                scanf("%d",&wyborGrupy);//wybieramy grupe do ktorej chcemy sie zapisac
                my_itoa(wyborGrupy,kolej.wiadomosc);
                kolej.type = 4;
                msgsnd(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),0);//wysylamy nasz wybor
                msgrcv(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),4,0);//odbieramy czy serwer odebral zadanie
                if(kolej2.value==1){ //jesli odebral to wysylamy mu nasze id
                    my_itoa(id,kolej.wiadomosc);
                    kolej.type=4;
                    msgsnd(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),0);
                }
                msgrcv(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),4,0);//odbieramy raport
               
                if(kolej2.value==1){//jesli "1" to zapisano nas do grupy
                    printf("Zapisano!\n");
                    mojeGrupy[wyborGrupy-1]=1;
                }else if(kolej2.value==-1){ //jesli "-1" to znaczy ze juz jestesmy w grupie
                    printf("Nalezysz juz do tej grupy!\n");
                }
                printf("\n");
                break;
            case 4://wypisywanie sie z grupy
                printf("Z ktorej grupy chcesz sie wypisac: \n");
                licznik=0;
                
                posiadaneGrupy[0]=posiadaneGrupy[1]=posiadaneGrupy[2]=posiadaneGrupy[3]=-1;
                for(i=0;i<GRUPY;i++){//wypisujemy grupy do ktorych nalezymy
                    if(mojeGrupy[i]==1){
                        printf("%d. grupa%d \n",(licznik+1),i);
                        posiadaneGrupy[licznik+1]=i;
                        licznik++;
                    }
                }
                if(licznik==0){
                    printf("Nie nalezysz do zadnej grupy!\n");
                    break;
                }
                scanf("%d",&wyborGrupy);//wybieramy grupe z ktorej chcemy sie wypisac
                temp = posiadaneGrupy[wyborGrupy];
                my_itoa(temp,kolej.wiadomosc);
                kolej.type=6;
                msgsnd(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),0);//wysylamy grupe z ktorej chcemy sie wypisac
                msgrcv(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),6,0);//otrzymujemy "1" jesli serwer przyjal zadanie
                if(kolej2.value==1){
                    my_itoa(id,kolej.wiadomosc);
                    kolej.type = 6;
                    msgsnd(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),0);//wysylamy serwerowi nasze id
                }
                mojeGrupy[temp]=0; //zaznaczamy sobie lokalnie ze wypisalismy sie z grupy
                break;
            case 5://wysylanie wiadomosci do uzytkownika
                
             printf("Do kogo chcesz wyslac wiadomosc? \nZalogowani: \n");
                kolej.type =7;
                my_itoa(id,kolej.wiadomosc);
                msgsnd(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),0);//wysylamy serwerowi nasze id, zeby wiedzial kto wysyla
                
                msgrcv(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),7,0);//odbieramy liczbe zalogowanych
            
                liczba = kolej2.value;
               
                strcpy(kolej.wiadomosc,"ok");
                msgsnd(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),0);//wysylamy "ok" ze przyjelismy liczbe
               
                for(i=0;i<liczba;i++){// otrzymujemy id zalogowanych i wypisujemy ich
                msgrcv(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),7,0);
                if(kolej2.value!=id){
                printf("%d. test%d\n",(i+1),(kolej2.value));
                }else{//jesli dostalismy nasze id to wypisujemy ze to my
                    printf("%d. test%d - ty\n",(i+1),(kolej2.value));
                }
                }
                int wyborUzytkownika;
                printf("Wybor: ");
                scanf("%d", &wyborUzytkownika);//wybieramy uzytkownika do ktoreog chcemy wyslac wiadomosc
                my_itoa((wyborUzytkownika-1),kolej.wiadomosc);
                kolej.type=7;
                msgsnd(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),0);//wysylamy uzytkownika do ktorego chcemy nadac
                msgrcv(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),7,0);//otrzymujemy "1" jesli serwer przyjal zadanie
                if(kolej2.value==1){
                    printf("Wprowadz wiadomosc (max 512 znakow):");
                    //scanf("%s",&wiad);
                    fgetc(stdin);
                    fgets(wiad,sizeof(wiad),stdin); //wprowadzamy wiadomosc
                    strcpy(msg.wiadomosc,wiad);
                    msg.type=10;
                    msgsnd(idMsg,&msg,sizeof(struct message)-sizeof(long),0);//wysylamy wiadomosc
                }
                printf("\n");
                break; 
                
            case 6://wysylanie wiadomosci do jednej z naszych grup
            
               printf("Do ktorej ze swoich grup chcesz wyslac wiadomosc: \n");
                licznik=0;
                posiadaneGrupy[0]=posiadaneGrupy[1]=posiadaneGrupy[2]=posiadaneGrupy[3]=-1;
                for(i=0;i<GRUPY;i++){//wypisujemy grupy do ktorych nalezymy
                    if(mojeGrupy[i]==1){
                        printf("%d. grupa%d \n",(licznik+1),i);
                        posiadaneGrupy[licznik+1]=i;
                        licznik++;
                    }
                }
                if(licznik==0){
                    printf("Nie nalezysz do zadnej grupy!\n");
                    break;
                }
                scanf("%d",&wyborGrupy);
                temp = posiadaneGrupy[wyborGrupy];
                my_itoa(temp,kolej.wiadomosc);
                kolej.type=8;
                msgsnd(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),0);//wysylamy wybrana grupe
                msgrcv(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),8,0);//otrzymujemy "1" jesli serwer przyjal nr grupy
                if(kolej2.value==1){
                    my_itoa(id,kolej.wiadomosc);
                    kolej.type = 8;
                    msgsnd(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),0);//wysylamy nasze id
                }
                msgrcv(idKolejki2,&kolej2,sizeof(struct kolejka2)-sizeof(long),8,0);//otrzymujemy "1" jesli serwer przyjal id
                if(kolej2.value==1){
                    printf("Wprowadz wiadomosc (max 512 znakow):");
                    fgetc(stdin);
                    fgets(wiadom,sizeof(wiadom),stdin);//wprowadzamy tresc
                    strcpy(msg.wiadomosc,wiadom);
                    msg.type=10;
                    msgsnd(idMsg,&msg,sizeof(struct message)-sizeof(long),0);//wysylamy tresc
                }
                
                break;
            case 7://wylogowywanie
            printf("do zobaczenia!\n");
            my_itoa(id,kolej.wiadomosc); 
            kolej.type = 3;
            msgsnd(idKolejki,&kolej,sizeof(struct kolejka)-sizeof(long),0);//wysylamy nasze id
            strcpy(msg.wiadomosc,"koniec,prosze wyloguj sie procesie odbierajacy wiadomosci");
            msg.type=id;
            msgsnd(idMsg,&msg,sizeof(struct message)-sizeof(long),0);//wysylamy procesowi odbierajacemu zeby sie wylaczyl
            exit(0);
            break;
            
        }
       
    }
    }
    }else exit(0);
}
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/sendfile.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>


#define SERVER_PORT 8080
#define SA struct sockaddr


char penguin_jpeg[]="HTTP/1.1 200 OK\r\n"
"Content-Type: image/jpeg\r\n\r\n";

char penguin_mp4[]="HTTP/1.1 200 OK\r\n"
"Content-Type: video/mp4\r\n\r\n";

char page_one[]= "HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html>"
"<html><head><title>Page 01</title></head>"
"<style type=text/css>"
"body {background-color: #C2A7F1;font-family: sans-serif;}"
"h1 {color: #2A1958; border-bottom: 1px solid #2A1958;}"
"p {margin-left: 100px;margin-right: 100px; text-align:justify}</style>"
"<body><h1><img src=\"penguin.jpeg\" width=\"325px\" height=\"450px\" style=\"margin-left:100px\"  /><br><br><center>Penguin</center></h1>"
"<p>Penguins (order Sphenisciformes /sfɪˈnɪsɪfɔːrmiːz/, family Spheniscidae /sfɪˈnɪsɪdiː/) are a group of aquatic flightless birds. They live almost exclusively in the Southern Hemisphere, with only one species, the Galápagos penguin, found north of the equator. Highly adapted for life in the water, penguins have countershaded dark and white plumage and flippers for swimming. Most penguins feed on krill, fish, squid and other forms of sea life which they catch while swimming underwater. They spend roughly half of their lives on land and the other half in the sea.Although almost all penguin species are native to the Southern Hemisphere, they are not found only in cold climates, such as Antarctica. In fact, only a few species of penguin live so far south. Several species are found in the temperate zone, but one species, the Galápagos penguin, lives near the equator. The largest living species is the emperor penguin (Aptenodytes forsteri):on average, adults are about 1.1 m (3 ft 7 in) tall and weigh 35 kg (77 lb). The smallest penguin species is the little blue penguin (Eudyptula minor), also known as the fairy penguin, which stands around 33 cm (13 in) tall and weighs 1 kg (2.2 lb).Among extant penguins, larger penguins inhabit colder regions, while smaller penguins are generally found in temperate or even tropical climates. Some prehistoric species attained enormous sizes, becoming as tall or as heavy as an adult human. These were not restricted to Antarctic regions; on the contrary, subantarctic regions harboured high diversity, and at least one giant penguin occurred in a region around 2,000 km south of the equator 35 mya, in a climate decidedly warmer than today.</p><br><br>"
"</body></html>";

char page_two[]= "HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html>"
"<html><head><title>Page 02</title></head>"
"<style type=text/css>"
"body {background-color: #C2A7F1;font-family: sans-serif;margin: 0 15%;}"
"p{margin-left: 100px;margin-right: 100px;text-align:justify}"
"h2 {color: #2A1958;font-size: 0.9em;}</style>"
"<body><br><br>"
"<h2>Etymology</h2>"
"<p text-align: justify>The word penguin first appears in the 16th century as a synonym for great auk.When European explorers discovered what are today known as penguins in the Southern Hemisphere, they noticed their similar appearance to the great auk of the Northern Hemisphere, and named them after this bird, although they are not closely related.</p>"
"<h2>Living species and recent extinctions</h2>"
"<p>The number of extant penguin species is debated. Depending on which authority is followed, penguin biodiversity varies between 17 and 20 living species, all in the subfamily Spheniscinae. Some sources consider the white-flippered penguin a separate Eudyptula species, while others treat it as a subspecies of the little penguin;the actual situation seems to be more complicated.Similarly, it is still unclear whether the royal penguin is merely a colour morph of the macaroni penguin. The status of the rockhopper penguins is also unclear. </p>"
"<center><video width=\"350\" height=\"350\" controls>"
"<source src=\"count.mp4\" type=video/mp4></video><br><br>"
"</body></html>";


int main()
{
   
  int one=1, listenfd, connfd, img, video, usr_input;
  struct sockaddr_in servaddr;
  char recvline[2048];
  
 
  printf("Enter 1 to load page one\nEnter 2 to load page two\n");
  scanf("%d",&usr_input);

  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    printf("socket error.");

   
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERVER_PORT);
   
  setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));
   
  if ((bind(listenfd,(SA *)&servaddr,sizeof(servaddr))) <0 )
  {
    printf("bind error\n");
    exit(0);
  }
   
  listen(listenfd,5);

  while(1)
  {
    connfd = accept(listenfd,(SA *)NULL,NULL);
    printf("got connection\n");
        
    if (connfd == -1) 
    {
      printf("Can't accept");
      continue;
    }

    if(usr_input==1)
    {
      if(!fork())
      {
        close(listenfd);
            
        memset(recvline,0,2048);
        read(connfd,recvline,2047);
            
        if(!strncmp(recvline,"GET /favicon.ico",80))
          img = open("favicon.ico",O_RDONLY);
               
        else if(strncmp(recvline,"GET /penguin.jpeg",80) >= 0)
        {
          img = open("penguin.jpeg",O_RDONLY);
          write(connfd,penguin_jpeg,sizeof(penguin_jpeg)-1);
          sendfile(connfd,img,NULL,41000);
          close(img);

        }
                
        else
          write(connfd,page_one,sizeof(page_one)-1);

        close(connfd);
        exit(1);
         
      }
      
    }
    if(usr_input==2)
    {
     if(!fork())
     {
       close(listenfd);
       memset(recvline,0,2048);
       read(connfd,recvline,2047);
       
       if(!strncmp(recvline,"GET /favicon.ico",80))
          img = open("favicon.ico",O_RDONLY);
               
       
       else if(strncmp(recvline,"GET /count.mp4",80) >= 0)
       {
          video = open("count.mp4",O_RDONLY);
          write(connfd,penguin_mp4,sizeof(penguin_mp4)-1);
          sendfile(connfd,video,NULL,41000);
          close(video);

       }
                
       else
          write(connfd,page_two,sizeof(page_two)-1);

       close(connfd);
       exit(1);
      }
    }

    

    close(connfd);
        
  }

close(listenfd);
return(0);

}

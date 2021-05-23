# SISOP Modul 3 #
## Anggota Kelompok ##

NRP | NAMA
------------- | -------------
05111940000111  | Evelyn Sierra
05111940000059  | Dido Fabian Fayed
05111940000139  | Zahrotul Adillah


## Soal 1 ##
Keverk adalah orang yang cukup ambisius dan terkenal di angkatannya. Sebelum dia menjadi ketua departemen di HMTC, dia pernah mengerjakan suatu proyek dimana keverk tersebut meminta untuk membuat server database buku. Proyek ini diminta agar dapat digunakan oleh pemilik aplikasi dan diharapkan bantuannya dari pengguna aplikasi ini. 

- Server.c 

Pada fungsi main terdapat fungsi untuk membuat file akun (akun.txt) 
Selanjutnya akan membuat socket baru pada fungsi socket_desc, jika gagal maka akan print output “Could not create socket”.
void *connection_handler(void *);
void *regis(void*);
```c 
int main(int argc , char *argv[])
{
   FILE* fp = fopen("akun.txt","a");
   fclose(fp);
 
   int socket_desc , new_socket , c , *new_sock;
   struct sockaddr_in server , client;
   char *message;
  
   //Create socket
   socket_desc = socket(AF_INET , SOCK_STREAM , 0);
   if (socket_desc == -1)
   {
       printf("Could not create socket");
   }
```
```c
//Prepare the sockaddr_in structure
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = INADDR_ANY;
   server.sin_port = htons( 8080 );
```
Fungsi untuk dapat melakukan bind dan listen pada server. Jika bind berhasil dilakukan akan muncul output ‘bind done’ apabila gagal maka akan di print ‘bind failed’.
```c
   if( bind(socket_desc,(struct sockaddr *) &server , sizeof(server)) < 0)
   {
       puts("bind failed");
       return 1;
   }
   puts("bind done");
   
   listen(socket_desc , 3);
```
Selanjutnya adalah fungsi untuk menerima incoming connection. Pada kondisi while untuk melakukan multi koneksi yang dilanjutkan dengan membuat  accept koneksi dari client. Semua kegiatan dilakukan pada fungsi connection_handler.
```c
//Accept and incoming connection
   puts("Waiting for incoming connections...");
   c = sizeof(struct sockaddr_in);

   while( (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
   {
       puts("Connection accepted");
      
       pthread_t sniffer_thread;
       new_sock = malloc(1);
       *new_sock = new_socket;
       
       if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
       {
           perror("could not create thread");
           return 1;
       }
```
Melakukan join pada thread sehingga tidak akan di terminate sebelum thread. Jika gagal akan print ‘accept failed’.
 ```c
       pthread_join( sniffer_thread , NULL);
       puts("Handler assigned");
   }
   if (new_socket<0){
       perror("accept failed");
       return 1;
   }
     return 0;
}
```
Fungsi connection_handler untuk menangani conection pada setiap client.
```c
void *connection_handler(void *socket_desc)
{
   int sock = *(int*)socket_desc;
   int valread;
 
   char menu[1024] = {0};
   char buffer[1024] = {0};
   char username[1024] = {0};
   char password[1024] = {0};
   char choice[1024] = {0};
```
Pada kondisi berikut, server akan mengirim data dari main menu dari client yang nantinya akan ditampilkan.
```c
while (1)
   {
       sprintf(menu, "\nMain Menu\n1. REGISTER\n2. LOGIN\nCHOICE : \e[s\n\e[u");
       write(sock, menu, 1024);
       // memset(menu, '0', 1024);
       bzero(menu, 1024);
 
       valread = read(sock, choice, 1024);
       if (valread < 1) return 0;
```
Jika 1 maka akan melakukan proses register.
```c
       if (!(strcmp(choice, "1")))
       {
           FILE* fp = fopen("akun.txt", "a");
           // printf("Masuk 1\n");
           sprintf(menu, "Username : \e[s\n\e[u");
           write(sock, menu, 1024);
           // memset(menu, '0', 1024);
           bzero(menu, 1024);
           valread = read(sock, username, 1024);
           if (valread < 1) return 0;
 
           sprintf(menu, "Password : \e[s\n\e[u");
           write(sock, menu, 1024);
           // memset(menu, '0', 1024);
           bzero(menu, 1024);
           valread = read(sock, password, 1024);
           if (valread < 1) return 0;
 
           fprintf(fp, "%s:%s\n", username, password);
           fclose(fp);
       }
 ```
 Jika 2 maka akan melakukan proses login.
```c
       else if (!(strcmp(choice, "2")))
       {
           //cek data
           char* aut = "0";
           //open file
           FILE* fp = fopen("akun.txt", "r");
           //selama gagal login dan belum login
           while (aut == "0")
           {
               // memset(menu, '0', 1024);
               bzero(menu, 1024);
               // printf("Masuk 2\n");
               sprintf(menu, "Username : \e[s\n\e[u");
               // printf("%s\n", menu);
               int ler = write(sock, menu, 1024);
               // if (ler == 0) printf("write\n");
               // memset(menu, '0', 1024);
               bzero(menu, 1024);
               valread = read(sock, username, 1024);
               if (valread < 1) return 0;
 
               sprintf(menu, "Password : \e[s\n\e[u");
               write(sock, menu, 1024);
               // memset(menu, '0', 1024);
               bzero(menu, 1024);
               valread = read(sock, password, 1024);
               if (valread < 1) return 0;
 
               while (fgets(buffer, 1024, fp) != NULL && aut == "0")
               {
                   char f_username[1024], f_password[1024];
                   char *token = strtok(buffer, ":");
                   strcpy(f_username, token);
                   token = strtok(NULL, "\n");
                   strcpy(f_password, token);
                  
                   if (strcmp(username, f_username) == 0 && strcmp(password, f_password) == 0)
                   {
                       // printf("MASUK!!!\n");
                       aut = "1";
                       write(sock, aut, 1024);
                       // printf("%s", aut);
                   }
               }
}
```
Apabila berhasil melakukan login maka akan dilakukan proses berikut
```c
           while (aut == "1")
           {
               // memset(menu, '0', 1024);
               bzero(menu, 1024);
               sprintf(menu, "LOGIN MENU\nadd\nsee\nfind\ndownload [namafile]\ndelete\nexit\nCHOICE : \e[s\n\e[u");
               write(sock, menu, 1024);
               // printf("flag\n");
               valread = read(sock, choice, 1024);
               if (valread < 1) return 0;
```
Apabila command dari client berhasil diterima (berupa add) maka akan melakukan proses command add yang selanjutnya akan membuka file tsv dan meminta inputan dari client berupa publisher, tahun publikasi, dan filepath. 
  ```c
               if (!strcmp(choice, "add"))
               {
                   FILE* fp_1 = fopen("files.tsv", "a");
                   char publisher[1024] = {0};
                   char tahun[1024] = {0};
                   char path[1024] = {0};
 
                   sprintf(publisher, "Publisher : \e[s\n\e[u");
                   write(sock, publisher, 1024);
                   // memset(publisher, '0', 1024);
                   bzero(publisher, 1024);
                   valread = read(sock, publisher, 1024);
                   if (valread < 1) return 0;
                  
                   sprintf(tahun, "Tahun Publikasi : \e[s\n\e[u");
                   write(sock, tahun, 1024);
                   // memset(tahun, '0', 1024);
                   bzero(tahun, 1024);
                   valread = read(sock, tahun, 1024);
                   if (valread < 1) return 0;
 
                   sprintf(path, "Filepath : \e[s\n\e[u");
                   write(sock, path, 1024);
                   // memset(path, '0', 1024);
                   bzero(path, 1024);
                   valread = read(sock, path, 1024);
                   if (valread < 1) return 0;          
```
Create directory FILES dan create data txt dari client. selanjutnya input yang berupa publisher, tahun publikasi dan filepath tadi akan dimasukkan ke file tsv.
```c                   
		   // transfer file
                   mkdir("FILES", 0777);
                   char pathServer[1024];
                   sprintf(pathServer, "/Users/didofabianfayed/Downloads/modul3/soal1/server/FILES/");
                   strcat(pathServer, path);
                   FILE* file_create = fopen(pathServer, "w");
 
                   // memset(buffer, '0', 1024);
                   bzero(buffer, 1024);
                   valread = read(sock, buffer, 1024);
                   if (valread < 1) return 0;
                   fprintf(file_create, "%s", buffer);
                   // fprintf(file_create, "\n");
 
                   fclose(file_create);
 
                   // memset(buffer, '0', 1024);
                   bzero(buffer, 1024);
                   sprintf(buffer, "\nFile Terkirim\n");
                   write(sock, buffer, 1024);
 
                   fprintf(fp_1, "%s\t%s\t%s\n", publisher, tahun, pathServer);
                   fclose(fp_1);
 
                   FILE* log = fopen("running.log", "a");
                   fprintf(log, "Tambah : %s %s\n", path, username);
                   fclose(log);
               }
```
Pada command see, server akan membuka file tsv dan akan mengambil data-data didalamnya.
```c
else if (!strcmp(choice, "see"))
               {
                   // memset(buffer, '0', 1024);
 
                   FILE* fcek = fopen("files.tsv", "r");
 
                   char    line[1024] = {0}, pub[1024] = {0}, thn[1024] = {0}, p_path[1024] = {0},
                           pathTemp[1024] = {0}, namafile[1024] = {0}, nama[1024] = {0}, extensi[1024] = {0};
                   int bukanawal = 0;
                   strcpy(buffer, "");
                   while (fgets(line, 1024, fcek))
                   {
                      // printf("ler\n");
                       // if (bukanawal)
                       strcat(buffer, "\n\n");
 
                       // ngambil tiap tab
                       char* token;
                       char samlekom1[1024];
                       strcpy(samlekom1, line);
                       char* kumsalam = samlekom1;
                       for (int i = 0; token = strtok_r(kumsalam, "\t", &kumsalam); i++)
                       {
                           if (i == 0)
                               strcpy(pub, token);
                           else if (i == 1)
                               strcpy(thn, token);
                           else if (i == 2)
                               strcpy(p_path, token);
                       }
                       strcpy(pathTemp, p_path);
 
                       // ngambil namafile.extensi
                       char samlekom2[1024];
                       strcpy(samlekom2, pathTemp);
                       kumsalam = samlekom2;
                       for (int i = 0; token = strtok_r(kumsalam, "/", &kumsalam); i++)
                       {
                           strcpy(namafile, token);
                       }
 
                       // misah namafile ama extensi
                       char samlekom3[1024];
                       strcpy(samlekom3, namafile);
                       kumsalam = samlekom3;
                       for (int i = 0; token = strtok_r(kumsalam, ".", &kumsalam); i++)
                       {
                           if (i == 0)
                               strcpy(nama, token);
                           else if (i == 1)
                           {
                               strcpy(extensi, ".");
                               strcat(extensi, token);
                           }
                       }
                       strcat(buffer, "Nama : ");
                       strcat(buffer, nama);
                       strcat(buffer, "\nPublisher : ");
                       strcat(buffer, pub);
                       strcat(buffer, "\nTahun Publishing : ");
                       strcat(buffer, thn);
                       strcat(buffer, "\nEkstensi File : ");
                       strcat(buffer, extensi);
                       strcat(buffer, "File Path : ");
                       strcat(buffer, p_path);
 
                       bukanawal++;
                   }
                   // printf("%s", buffer);
                   write(sock, buffer, 1024);
                   bzero(buffer, 1024);
                   // memset(buffer, '0', 1024);
               }
```
Pada command download berikut, digunakan untuk mengunduh buku dari server ke client sesuai dengan penamaannya (download [namafile].[extensi]). Misalkan terdapat buku abc.tsv dapat didownload dengan command ‘download abc.tsv’. Apabila tidak ditemukan akan menampilkan output ‘buku tidak ditemukan’.
```c
else if (!strncmp(choice, "download", 8))
               {
                   bzero(buffer, 1024);
                   char temp[1024], buku[1024];
                   strcpy(temp, choice);
                   // printf("CHOICE : %s\n", choice);
                   char* save = temp;
                   char* token;
                   for (int i = 0; token = strtok_r(save, " ", &save); i++)
                   {
                       bzero(buku, 1024);
                       strcpy(buku, token);
                   }
                   int found = 0;
                   printf("buku : %s\n", buku);
                  
                   if (!strcmp(buku, ""))
                   {
                       strcpy(buffer, "masukkan nama buku");
                       write(sock, buffer, 1024);
                       bzero(buffer, 1024);
                   }
                   else
                   {
                       char pathserver[1024] = "/Users/didofabianfayed/Downloads/modul3/soal1/server/FILES/";
                       strcat(pathserver, buku);
                       printf("pathserver : %s\n", pathserver);
 
                       FILE* file = fopen("files.tsv", "r");
                       char line[1024];
 
                       while(fgets(line, 1024, file))
                       {
                           char temp[1024] = {0}, data[1024] = {0};
                           strcpy(temp, line);
                           char* t;
                           char* simpen = temp;
                           for (int i = 0; t = strtok_r(simpen, "\t", &simpen); i++)
                           {
                               strncpy(data, t, strlen(t) - 1);
                           }
                           printf("data : %s\n", data);
                           if (!strcmp(pathserver, data))
                           {
                               found = 1;
                               break;
                           }
                       }
                       if (found)
                       {
                           FILE* ffile = fopen(pathserver, "rb");
                           char dats[1024] = {0};
                          
                           while (fgets(dats, 1024, ffile) != NULL)
                           {
                               write(sock, dats, strlen(dats));
                               bzero(dats, 1024);
                           }
                           fclose(ffile);
                       }
                       else
                       {
                           strcpy(buffer, "buku tidak ditemukan");
                           write(sock, buffer, 1024);
                           bzero(buffer, 1024);
                       }
                   }
               }
```
Pada command delete berikut, digunakan untuk menghapus data buku pada server. Dapat dilakukan dengan command ‘delete [namafile].[extensi]’. Misalkan terdapat buku abc.tsv dapat dihapus dengan command ‘delete abc.tsv’. Apabila tidak ditemukan akan menampilkan output ‘buku tidak ditemukan’.
```c
else if (!strncmp(choice, "delete", 6))
               {
                   bzero(buffer, 1024);
                   char temp[1024] = {0}, buku[1024]= {0};
                   strcpy(temp, choice);
                   char* token;
                   char* save = temp;
                   for (int i = 0; token = strtok_r(save, " ", &save); i++)
                   {
                       strcpy(buku, token);
                   }
                   char pathserver[1024] = "/Users/didofabianfayed/Downloads/modul3/soal1/server/FILES/";
                   strcat(pathserver, buku);
 
                   if (!strcmp(buku, ""))
                   {
                       strcpy(buffer, "masukkan nama buku");
                   }
                   else
                   {
                       FILE* cekfile = fopen(pathserver, "r");
                       if (!cekfile)
                       {
                           strcpy(buffer, "buku tidak ditemukan");
                       }
                       else
                       {
                           FILE* file1 = fopen("files.tsv", "r");
                           FILE* file2 = fopen("temp.tsv", "w");
 
                           char line[1024] = {0};
                           while (fgets(line, 1024, file1))
                           {
                               char motong[1024], pathbukunya[1024];
                               strcpy(motong, line);
                               char* potong = motong;
                               char* token;
                               for (int i = 0; token = strtok_r(potong, "\t", &potong); i++)
                               {
                                   strncpy(pathbukunya, token, strlen(token) - 1);
                               }
                               char templine[1024];
                               strcpy(templine, line);
                               if (strcmp(pathserver, pathbukunya))
                               {
                                   printf("pathserver : %s\npathbukunya : %s\n\n", pathserver, pathbukunya);
                                   fprintf(file2, "%s", templine);
                               }
                           }
                           fclose(file1);
                           fclose(file2);
                           remove("files.tsv");
                           rename("temp.tsv", "files.tsv");
 
                           char old[1024] = "/Users/didofabianfayed/Downloads/modul3/soal1/server/FILES/old-";
                           strcat(old, buku);
                           rename(pathserver, old);
 
                           strcpy(buffer, "buku sukses dihapus");
                       }
                       fclose(cekfile);
 
                       FILE* log = fopen("running.log", "a")       ;
                       fprintf(log, "Hapus : %s %s\n", buku, username);
                       fclose(log);               
                   }
                   write(sock, buffer, 1024);
                   bzero(buffer, 1024);
               }
               else if (!strncmp(choice, "find", 4))
               {
                   char temp[1024], namabuku[1024];
                   strcpy(temp, choice);
                   char* p_temp = temp;
                   char* token;
                   for (int i = 0; token = strtok_r(p_temp, " ", &p_temp); i++)
                   {
                       strcpy(namabuku, token);
                   }
                   if (!strcmp(namabuku, ""))
                   {
                       strcpy(buffer, "masukkan nama buku");
                   }
                   else
                   {
                       FILE* fcek = fopen("files.tsv", "r");
 
                       char    line[1024] = {0}, pub[1024] = {0}, thn[1024] = {0}, p_path[1024] = {0},
                               pathTemp[1024] = {0}, namafile[1024] = {0}, nama[1024] = {0}, extensi[1024] = {0};
                       int bukanawal = 0;
                       strcpy(buffer, "");
                       while (fgets(line, 1024, fcek))
                       {
                           // printf("ler\n");
                           // if (bukanawal)
                           strcat(buffer, "\n\n");
 
                           // ngambil tiap tab
                           char* token;
                           char samlekom1[1024];
                           strcpy(samlekom1, line);
                           char* kumsalam = samlekom1;
                           for (int i = 0; token = strtok_r(kumsalam, "\t", &kumsalam); i++)
                           {
                               if (i == 0)
                                   strcpy(pub, token);
                               else if (i == 1)
                                   strcpy(thn, token);
                               else if (i == 2)
                                   strcpy(p_path, token);
                           }
                           strcpy(pathTemp, p_path);
 
                           // ngambil namafile.extensi
                           char samlekom2[1024];
                           strcpy(samlekom2, pathTemp);
                           kumsalam = samlekom2;
                           for (int i = 0; token = strtok_r(kumsalam, "/", &kumsalam); i++)
                           {
                               strcpy(namafile, token);
                           }
 
                           // misah namafile ama extensi
                           char samlekom3[1024];
                           strcpy(samlekom3, namafile);
                           kumsalam = samlekom3;
                           for (int i = 0; token = strtok_r(kumsalam, ".", &kumsalam); i++)
                           {
                               if (i == 0)
                                   strcpy(nama, token);
                               else if (i == 1)
                               {
                                   strcpy(extensi, ".");
                                   strcat(extensi, token);
                               }
                           }
                           // printf("%s\n%s\n%s\n%s\n%s\n", nama, pub, thn, extensi, p_path);
                           if (!strstr(nama, namabuku)) continue;
 
                           strcat(buffer, "Nama : ");
                           strcat(buffer, nama);
                           strcat(buffer, "\nPublisher : ");
                           strcat(buffer, pub);
                           strcat(buffer, "\nTahun Publishing : ");
                           strcat(buffer, thn);
                           strcat(buffer, "\nEkstensi File : ");
                           strcat(buffer, extensi);
                           strcat(buffer, "\nFile Path : ");
                           strcat(buffer, p_path);
 
                           bukanawal++;
                       }
                       if (!bukanawal)
                       {
                           strcpy(buffer, "buku tidak ditemukan");
                       }
                   }
                   write(sock, buffer, 1024);
                   bzero(buffer, 1024);
               }
           }
           fclose(fp);
       }
   }
      
   //Free the socket pointer
   free(socket_desc);
  
   return 0;
}
```
- Client.c


### Hasil Output ###

- 1. Register, login dan add
![register, login, add](https://user-images.githubusercontent.com/80528848/119257904-979b9380-bbf1-11eb-841c-e8ab0d9ca5de.png)

- 2. Penampakan folder server
![penampakan folder server](https://user-images.githubusercontent.com/80528848/119257927-b6018f00-bbf1-11eb-91a0-6845f5d38cbd.png)

- 3. Download
![download](https://user-images.githubusercontent.com/80528848/119257952-d3cef400-bbf1-11eb-9b5c-64fb8e3f89a8.png)

- 4. See, find
![see, find](https://user-images.githubusercontent.com/80528848/119257936-bef26080-bbf1-11eb-9173-50f39dc4d3cd.png)

- 5. Delete
![+delete](https://user-images.githubusercontent.com/80528848/119257964-ddf0f280-bbf1-11eb-8bb8-1019e765f68d.png)

- 6. Penampakan file pada FILES yang telah di delete
![penampakan file pada server seletah didelete](https://user-images.githubusercontent.com/80528848/119257965-e0ebe300-bbf1-11eb-9b7b-7bd7d0376eba.png)



## Soal 2 ##
Crypto (kamu) adalah teman Loba. Suatu pagi, Crypto melihat Loba yang sedang kewalahan mengerjakan tugas dari bosnya. Karena Crypto adalah orang yang sangat menyukai tantangan, dia ingin membantu Loba mengerjakan tugasnya. Detil dari tugas tersebut adalah:
- a. Membuat program perkalian matrix (4x3 dengan 3x6) dan menampilkan hasilnya. Matriks nantinya akan berisi angka 1-20 (tidak perlu dibuat filter angka).
- b. Membuat program dengan menggunakan matriks output dari program sebelumnya (program soal2a.c) (Catatan!: gunakan shared memory). Kemudian matriks tersebut akan dilakukan perhitungan dengan matrix baru (input user) sebagai berikut contoh perhitungan untuk matriks yang ada. Perhitungannya adalah setiap cel yang berasal dari matriks A menjadi angka untuk faktorial, lalu cel dari matriks B menjadi batas maksimal faktorialnya (dari paling besar ke paling kecil) (Catatan!: gunakan thread untuk perhitungan di setiap cel). 
- Ketentuan :		
-- `If a >= b  -> a!/(a-b)!`
-- `If b > a -> a!`
-- `If 0 -> 0`
- c. Karena takut lag dalam pengerjaannya membantu Loba, Crypto juga membuat program (soal2c.c) untuk mengecek 5 proses teratas apa saja yang memakan resource komputernya dengan command “ps aux | sort -nrk 3,3 | head -5” (Catatan!: Harus menggunakan IPC Pipes)
### Pembahasan ###
#### soal2a ####
```c
#define baris1 4
#define kolom1 3
#define baris2 3
#define kolom2 6

int mx1[baris1][kolom1], mx2[baris2][kolom2];
int i,j,k;
```
Pertama-tama inisialisasi untuk baris, kolom, matriks, beserta variabel yang dipakai untuk looping
```c
void matriksAwal(){
	printf("Input matriks 4x3\n");
	for (i = 0; i < baris1; i++) {
    	for (j = 0; j < kolom1; j++) {
      		scanf("%d", &mx1[i][j]);
   	 	}
  	}
  	printf("Input matriks 3x6\n");
	for (i = 0; i < baris2; i++) {
    	for (j = 0; j < kolom2; j++) {
      		scanf("%d", &mx2[i][j]);
   	 	}
  	}
}
```
Fungsi di atas bertujuan untuk memasukkan input matriks pertama dan kedua dari user pada tiap baris dan kolomnya
```c
    key_t key = 1234;
  	int (*value)[kolom2];

  	int shmid = shmget(key, sizeof(int[4][6]), IPC_CREAT | 0666);
  	value = shmat(shmid, NULL, 0);
    
    //Looping untuk hitung dan print hasil
    
    shmdt(value);
```
Setelah memanggil fungsi untuk memasukkan input, dibuat shared memory untuk matriks sesuai template yang ada di modul dengan beberapa penyesuaian
```c
printf("Hasil Perkalian Matriks :\n");
  	for (i = 0; i < baris1; i++) {
    	for (j = 0; j < kolom2; j++) {
    		for (k = 0; k< 3; k++){
			value[i][j]=0;
      			value[i][j] += mx1[i][k] * mx2[k][j];
			}
      		printf("%d ",value[i][j]);
   	 	}
    	printf("\n");
  	}
```
Lalu pada looping di atas, terdapat perhitungan hasil dari perkalian dua matriks beserta print outputnya

#### soal2b ####
```c
#define baris 4
#define kolom 6

int mx2[baris][kolom], selisih, i, j;
bool isnull = false;
```
- Define baris dan kolom untuk matriks yaitu 4x6
- inisialisasi matriks inputan
- inisialisasi selisih yang berfungsi untuk menghitung selisih dari matriks pertama(dari hasil soal2a) - kedua(inputan)
- inisialisasi i dan j untuk variabel loopin
- inisialisasi variabel boolean isnull untuk mengetahui apakah ada nilai 0 pada matriks pertama atau kedua
```c
int main(){
	int angka;
	key_t key = 1234;
    int (*value)[kolom];
    int shmid = shmget(key, sizeof(int[4][6]), IPC_CREAT | 0666);
    value = shmat(shmid, NULL, 0);
    
    //isi
    
    shmdt(value);
    shmctl(shmid, IPC_RMID, NULL);
```
- Pertama pada main, membuat shared memory untuk mengambil nilai hasil dari soal2a
- Setelah isi dijalankan, shared memory akan dihancurkan dengan `shmctl`
```c
void input(){
	printf("Input matriks 4x6\n");
	for (i = 0; i < baris; i++) {
    	for (j = 0; j < kolom; j++) {
      		scanf("%d", &mx2[i][j]);
   	 	}
  	}
}
```
Setelah itu, memanggil fungsi input untuk input matriks kedua dari user
```c
pthread_t tid[baris*kolom];
    int index=0;
    for(i = 0; i < baris; i++){
        for(j = 0; j < kolom; j++){
        	isnull=false;
            long long *val = malloc(sizeof(long long[4][6]));
            *val = value[i][j];
            selisih = value[i][j] - mx2[i][j];
            if(value[i][j]==0 || mx2[i][j]==0) isnull=true;
            pthread_create(&tid[index], NULL, &kondisi, val);
            sleep(1);
            index++;
        }
        printf("\n");
    }
    for (i = 0; i < index; i++) {
        pthread_join(tid[i], NULL);
    }
```
- Membuat thread sebanyak `baris*kolom` yaitu `4x6 = 24`
- Variabel index yang bertambah pada tiap thread
- Alokasi memori untuk variabel `*val` dan diisi dengan matriks pertama (hasil dari soal2a)
- Selisih untuk menghitung selisih dari matriks pertama(dari hasil soal2a) - kedua(inputan)
- `isnull` diset `true` ketika menemui nilai 0 pada matriks pertama atau kedua
- Membuat thread dengan menjalankan fungsi kondisi sebagai routine dengan atribut val sebagai variabel yang digunakan  
- `sleep(1)` supaya thread berjalan 1 detik setelah thread sebelumnya ini berfungsi agar thread dapat berjalan berurutan
- print `\n` setiap kolom selesai
- Join setiap thread yang telah dibuat dengan `pthread_join(tid[i], NULL);`
- Fungsi yang digunakan : 
```c
long long factorial(int n) {
    if (n == 0) return 1;
    else return n * factorial(n - 1);
}

long long except(int n){
	if (n == selisih) return 1;
        else return n * except(n - 1);
}

void *kondisi(void* arg){
	long long angka = *(long long*)arg;
	if(isnull)//=0
		printf("0\t");
	else if(selisih<1) /// <1
    	printf("%lld\t", factorial(angka));
	else//>1
		printf("%lld\t", except(angka));
}
```
Saat membuat thread, menjalankan fungsi kondisi dengan rincian sebagai berikut :
- Jika ada nilai 0, maka akan menampilkan `0`
- Jika `selisih<1` atau bermakna nilai pada matriks 1 >= nilai pada matriks 2 pada index tertentu, maka akan menampilkan hasil dari fungsi `factorial` dengan parameter isi matriks 1 pada index tersebut
- Selain itu, akan menampilkan hasil dari fungsi `except` dengan parameter isi matriks 1 pada index tersebut
- Pada fungsi `factorial`, akan dilakukan perhitungan faktorial dari n sampai 1
- Pada fungsi `except`, akan dilakukan perhitungan faktorial dari n sampai selisih+1

#### soal2c ####
```c
int pid;
int fd1[2];
int fd2[2];
```
inisialisasi pid, `fd1` untuk pipe pertama dan `fd2` untuk pipe kedua 
```c
void main() {
	pid_t child_id;
  	// create pipe1
  	if (pipe(fd1)==-1) 
	{ 
		exit(1); 
	} 
```
- Inisialisasi `child_id`
- Membuat pipe 1, jika gagal maka akan di-exit
```c
child_id = fork();
  	if (child_id < 0)
    {
        exit(EXIT_FAILURE);
    }
	  else if (child_id == 0) {
    	// stdin --> ps --> pipe1
    	// input from stdin (already done)
  		// output to fd1
  		dup2(fd1[1], 1);
  		// close fds
  		close(fd1[0]);
  		close(fd1[1]);
  		// exec
  		char *arg[] = {"ps", "aux", NULL};
  		execv("/bin/ps", arg);
  		// exec didn't work, exit
  		_exit(1);
  	}
```
- Membuat fork
- Jika gagal, maka akan di-exit
- Pada child akan dijalankan perintah `ps aux` dan menulis output pada pipe 1 
```c
else{
  		// create pipe2
  		if (pipe(fd2) == -1) {
			exit(1); 
  		}
	
		child_id = fork();
  		// fork (grep root)
  		if (child_id < 0)
   	 	{
        	exit(EXIT_FAILURE);
    	}
		else if (child_id == 0) {
    		// fd1 --> grep --> pipe2
    		// input from fd1
  			dup2(fd1[0], 0);
		  	// output to fd2
		  	dup2(fd2[1], 1);
		  	// close fds
		  	close(fd1[0]);
		  	close(fd1[1]);
		  	close(fd2[0]);
		  	close(fd2[1]);
		  	// exec
		  	char *arg[] = {"sort", "-nrk", "3.3",NULL};
		  	execv("/bin/sort", arg);
		  	// exec didn't work, exit
		  	_exit(1);
  		}
  		// parent
		else{
  			// close unused fds
  			close(fd1[0]);
  			close(fd1[1]);

    		// pipe2 --> grep --> stdout
    		// input from fd2
		  	dup2(fd2[0], 0);
		  	// output to stdout (already done)
		  	// close fds
		  	close(fd2[0]);
		  	close(fd2[1]);
		  	// exec
		  	char *arg[] = {"head","-5",NULL}; 
		  	execv("/bin/head",arg);
		  
		  	// exec didn't work, exit
		  	_exit(1);	
		}
	}
}
```
Pada parent fork pertama, akan berjalan :
- Membuat pipe 2, jika gagal maka akan di-exit
- Membuat fork kedua
- Pada child fork kedua, akan dijalankan perintah `sort -nrk 3.3` dan menuliskan output pada pipe kedua 
- Pada parent fork kedua, akan dijalankan perintah `head -5` dan menuliskan output pada stdout
#### Screenshoot Hasil ####
- Hasil pada 2a dan 2b

![2a2b](https://user-images.githubusercontent.com/72771774/119224933-1083e800-bb2b-11eb-9428-aade157cfef0.PNG)

- Hasil pada 2c dibandingkan dengan langsung input di terminal akan sama

![2c](https://user-images.githubusercontent.com/72771774/119224934-124dab80-bb2b-11eb-8699-c88b69e46b33.PNG)
## Soal 3 ##
Pada soal 3 ini, kita perlu membuat program yang menerima perintah `-f`, `-d`, dan `*`.
Perintah `-f` artinya mengkategorikan file dengan ekstensi yang sama, dengan nama folder menggunakan nama ekstensi file yang sama.

Pertama, kita harus membuat function yang bisa mengecek apakah path yang dimasukkan memiliki file atau tidak
```c
int is_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}
```
Pada main program, kita membuat thread dan memanggil function `fcommand()` sebagai penghubung.
```c
void *fcommand(void* arg)
{
    args_struct args = *(args_struct*) arg;
    movefileF(args.asal, args.cwd);
    pthread_exit(0);

}
```
Selanjutnya gunakan function `movefileF()` untuk memindahkan file ke dalam folder baru, dengan nama folder barunya adalah nama ekstensi file tersebut
```c
void movefileF(char *argv, char *cwd)
{
    int hiddenfile = 0;
    char string[500];
    strcpy(string, argv);

    int isfile = is_file(string);
    char* tipe = strrchr(string, '.');
    char* nama = strrchr(string, '/');

    if(nama[1] == '.')
    {
        hiddenfile = 1; //is hidden file
    }

    char tipeLow[100];
    if(tipe)
    {
            strcpy(tipeLow, tipe);
            for(int i = 0; tipeLow[i]; i++){
                tipeLow[i] = tolower(tipeLow[i]);
            }
    }
    else{
        if(!isfile)
        {
            printf("This is folder\n");
        }
        else
        {
            strcpy(tipeLow, " Hidden"); // no extention
        }
    }

        if(hiddenfile)
        {
            strcpy(tipeLow," Hidden");
        }
        mkdir((tipeLow + 1), 0777);

        size_t len = 0 ;
        char a[500] ;
        strcpy(a, argv);
        char b[500] ;
        strcpy(b, cwd);
        strcat(b, "/");
        strcat(b, tipeLow+1);
        strcat(b, nama);
        if (!rename(a,b)) {
            printf("Berhasil Dikategorikan\n");
        }
        else
        {
            printf("Sad, Gagal :(\n");
        }
        remove(a);
    
}
```
Perintah `-d` juga memiliki function yang sama dengan `-f`, bedanya adalah pengkategorian direktori dan output hasilnya
```c
    else if(strcmp(argv[1], "-d") == 0)
    {
        char asal[1000];
        strcpy(asal,argv[2]);
        sortfile(asal);
    }
```
Pada main program, akan memanggil function `sortfile()` untuk mengelompokkan folder yang diinginkan dan dibaca file yang didalamnya
```c
void sortfile(char *asal)
{
    args_struct args;
    flag = 1;
    strcpy(args.cwd, "/Users/evelynsierra/ITS/Semester_4/Sisop/soal-shift-sisop-modul-3-B04-2021-main/soal3/soal3");
    DIR *dirp;
    struct dirent *entry;
    dirp = opendir(asal);
    int index = 0;
    while((entry = readdir(dirp)) != NULL)
    {
        if(entry->d_type == DT_REG)
        {
            char filename[305];
            sprintf(filename,"%s/%s",asal, entry->d_name);
            strcpy(args.asal,filename);

            if(strcmp(filename,"/Users/evelynsierra/ITS/Semester_4/Sisop/soal-shift-sisop-modul-3-B04-2021-main/soal3/soal3/soal3.c") != 0)
            {
                pthread_create(&tid[index], NULL, dcommand, (void *)&args);
                printf("%s\n", filename);
                sleep(1);
                index++;
            }
        }
    }

    if(!flag)
    {
        printf("Yah gagal disimpan\n");
    }
    else
    {
        printf("Direktori sudah disimpan\n");
    }
}
```

Setelah itu, memanggil function `dcommand` dan `movefileD`.
```c
void *dcommand(void* arg)
{
    args_struct args = *(args_struct*) arg;
    movefileD(args.asal, args.cwd);
    pthread_exit(0);

}
```

```c
void movefileD(char *argv, char *cwd)
{
    int hiddenfile = 0;
    char string[500];
    strcpy(string, argv);
    int isFile = is_file(string);
    char* tipe = strrchr(string,'.');
    char* nama = strrchr(string, '/');

    if(nama[1] == '.')
    {
        hiddenfile = 1;
    }

    char tipeLow[100];
    if(tipe)
    {
            strcpy(tipeLow, tipe);
            for(int i = 0; tipeLow[i]; i++){
                tipeLow[i] = tolower(tipeLow[i]);
            }
    }
    else
    {
        if(!isFile)
        {
            printf("This is a folder\n");
            return;
        }
        else
        {
            strcpy(tipeLow, " Unknown"); //no extention
        }
    }

    if(hiddenfile)
    {
        strcpy(tipeLow, " Hidden");
    }
    mkdir((tipeLow + 1), 0777);

     size_t len = 0 ;
        char a[1000] ;
        strcpy(a, argv);
        char b[1000] ;
        strcpy(b, cwd);
        strcat(b, "/");
        strcat(b, tipeLow+1);
        strcat(b, nama);
        if (rename(a,b)) 
        {
            flag=0;
        }

        remove(a);
}
```
Dan yang terakhir adalah perintah `*`, alur pemanggilannya sama dengan perintah `-d`. Bedanya adalah alamat folder yang dikirimkan adalah alamat folder tetap.

```c
    else if(strcmp(argv[1], "*") == 0)
    {
        char asal[] = "/Users/evelynsierra/ITS/Semester_4/Sisop/soal-shift-sisop-modul-3-B04-2021-main/soal3/soal3";

        sortfile(asal);
    }
```
#### Screenshoot Hasil ####
![2](https://user-images.githubusercontent.com/55088939/119247118-5c797000-bbb1-11eb-9bc4-4c4860184e17.jpg)
![3](https://user-images.githubusercontent.com/55088939/119247120-5daa9d00-bbb1-11eb-97b6-559d6fe755ee.jpg)
![4](https://user-images.githubusercontent.com/55088939/119247121-5e433380-bbb1-11eb-9a79-69331ccd81d9.jpg)
![6](https://user-images.githubusercontent.com/55088939/119247122-5edbca00-bbb1-11eb-9dcf-d391d159d914.jpg)



## Kendala ##
 - Tidak mengeluarkan hasil output berupa berhasil/gagal dikategorikan
 - Belum bisa mengelompokkan extention berupa angka
 - File di dalam folder belum terpindah karena belum memakai `/`

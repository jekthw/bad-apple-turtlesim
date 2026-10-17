# bad-apple-turtlesim
<img width="520" height="558" alt="image" src="https://github.com/user-attachments/assets/570c44bc-bb2f-4805-9e7f-6a81230a8449" />

### me when the apple is of subpar quality

repo ini berisi kode C++ ROS2 untuk menggambar frame ke-83 dari "Bad Apple!"
program ini bisa digunakan untuk menggambar apapun selama diberikan waypoint koordinat bernilai 0-11 yang tepat dari pengguna berupa file "waypoints.txt"
DIBUAT SEBAGAI PENUGASAN MODUL 2 INTERNSHIP ROBOCOON

### how to cara memakai use
- Buat file dengan nama "waypoints.txt" pada directory yang sama dengan program
- Isi file tersebut dengan data koordinat yang ingin digambar
- Gunakan -1 -1 untuk mengangkat pen jika ingin membuat bidang baru yang terpisah (jangan lupa taruh di awal file juga agar si kura-kura tidak menarik garis dari origin menuju titik pertama)

**Ultra Max Extreme Professional quality of life tip:** 
Gunakan web seperti plotextract.com untuk menggambar titik secara manual (tidak perlu terlalu akurat) lalu download sebagai .csv dan extract data yang diperlukan menggunakan Google Sheet / Excel. Jangan lupa ubah karakter tab dengan karakter spasi jika menyalin dari kedua program tersebut.
<img width="1706" height="1002" alt="image" src="https://github.com/user-attachments/assets/872b43be-4310-442d-bf1e-6b9e9a647fff" />

### keunggulan
- bisa menggambar gambar apa saja asal berupa outline monokrom + pengguna tidak malas menggambar gambar tersebut
- bisa membuat bidang terpisah
- sudah menggunakan PID jadi garisnya tidak menccenggzc
### kelemahan
- developer tidak tau cara membuat algoritma untuk otomasi perubahan gambar apapun menjadi data titik

# how did we get here?
<img width="648" height="142" alt="image" src="https://github.com/user-attachments/assets/72ce96c1-7f95-499e-b15e-6b0e7231ee28" />

proyek ini dimulai sebagai penugasan modul 2 internship robocon dimana kita ditugaskan untuk membuat program untuk membuat gambar di turtlesim ROS2, yang hanya menerima masukkan berupa seberapa cepat ia berjalan per detik nya dan seberapa cepat ia berputar pada porosnya. tantangannya terletak dalam mengubah suatu gambar/data titik yg kita punya menjadi gerakan dan putaran tersebut.

## if it has a screen, you can put bad apple on it
aku awalnya bingung he iki nggambar opo yo aduh hmm lalu saya terpikirkan eh bad apple kyke lucu  jadi akhirnya itu yg saya tetapkan sebagai tujuan. tapi tentunya menghardcode titik gambar satu-satu pada barisan program itu melelahkan woy nah terpikirkanlah saya oh gimana kalau gambar nya saya buat dulu di program eksternal, saya extract data data titik nya secara berurutan, terus program itu bisa menerima data titik titik tersebut dan diolah menjadi perintah bergerak dan berputar untuk si kura-kura

## DRAW ANYTHINGKNGGGGGG
mekanismu input file
```cpp
void loadWaypoints(const std::string& filepath) {
        std::ifstream file(filepath);
        double x, y;
        while (file >> x >> y) waypoints.push_back({x, y});
        file.close();
        if (!waypoints.empty()) {
            if (waypoints[0].first < 0.0) {
                setPen(false);      
                activatePen = true; 
                waypointIdx = 1;    
            } else {
                waypointIdx = 0;
            }
            if(waypointIdx<waypoints.size()) {
                goalX = waypoints[waypointIdx].first;
                goalY = waypoints[waypointIdx].second;
            }
        }
    }
```
prosedur di atas bertugas untuk melakukan looping dari awal file hingga EOF, dan push_back nilai semua x,y sebagai `pair<double,double>` ke dalam vector `waypoints`.
terdapat pengecekan kecil juga untuk memastikan turtle tidak bergerak ke koordinat {-1 -1} segera setelah membaca nilai pertama (yang biasanya bernilai {-1-1} untuk mematikan pen sementara)


## i put the D In PID
mekanisme utama publish perintah pergerakan
```cpp
    void posCallback(turtlesim::msg::Pose::SharedPtr msg) {
        
        double distX = goalX - msg->x, distY = goalY - msg->y;
        double dist = sqrt(distX*distX + distY*distY);
        geometry_msgs::msg::Twist message;
        if(dist > 0.1) {
            double angle = std::atan2(distY, distX) - msg->theta;
            angle = std::atan2(std::sin(angle), std::cos(angle));
            message.angular.z = 4.5*angle;
            if(fabs(angle) < 0.4) {
                message.linear.x = 3.0*dist;
            }
            pub->publish(message);
        }
        else {
            if(activatePen) {
                setPen(1);
                activatePen = 0;
            }
            waypointIdx++;

            if(waypointIdx < waypoints.size() && waypoints[waypointIdx].first < 0.0) {
                setPen(0); 
                waypointIdx++;
                activatePen = 1;
            }
            if(waypointIdx < waypoints.size()) {
                goalX = waypoints[waypointIdx].first;
                goalY = waypoints[waypointIdx].second;
            }
            else {
                geometry_msgs::msg::Twist stop;
                pub->publish(stop);
                rclcpp::shutdown();
            }
        } 
        
    }
```
program ini bergerak berdasar event-based trigger. artinya setiap kali ia menerima callback dari `turtlesim::msg::Pose` (dalam artian, terjadi update pada data `Pose` atau posisi kura-kura), ia akan mengkalkulasikan gerakan yang diperlukan untuk pergi ke waypoint selanjutnya. dimana nilai `dist` merupakan euclidean distance yang didapat menggunakan teorema pythagoras. dan didapatkan sudut selisih dari arah kura-kura sekarang menuju waypoint dengan arctangent dari y/x tujuan dikurangi theta saat ini. nilai ini perlu kita normalisasi juga agar berputarnya tidak berlebihan saat transisi sudut (misal dari `pi` ke `-pi` yang seharusnya bernilai sama tapi akan menyebabkan kura-kura berputar 360 derajat)/

PID nya terletak pada bagaimana program ini memerintah kura-kura untuk bergerak, dimana ia mnenyatakan `message.linear.x = 3.0*dist;`. 
Kecepatan kura-kura berubah secara proporsional dengan jaraknya sekarang dikali suatu nilai proportional gain yang kita sesuaikan sendiri agar ia tidak terlalu lambat dalam bergerak, namun tidak sampai overshoot juga.
hal yang sama terjadi pada `message.angular.z = 4.5*angle;` dimana kecepatan berputar kura-kura berubah secara proporsional terhadap kekurangan sudut dirinya pada waypoint.  

program akan mengecek bila posisinya belum mendekati tujuan `(dist > 0.1)`, ia akan melakukan kalkulasi sudut yang perlu ia putar agar menghadap waypoint, lalu berjalan menuju titik tersebut.



Aku teringat program serupa yang pernah aku program di game Mindustry—ya gak serupa juga tapi intinya ada objek yang diberikan suatu titik tujuan lalu dia bergerak menuju titik tersebut.
Caranya adalah kita menghitung vektor arah dari titik lokasi saat ini menuju koordinat target. 
Dari selisih koordinat itu (`distX` & `distY`), kita dapet jarak dan sudut yang harus dihadap si kura-kura menggunakan `atan2(distY, distX)` (arc tangent 2). 
lalu agar berputarnya tidak berlebihan saat transisi sudut (misal dari `pi` ke `-pi` yang seharusnya bertitik sama tapi kura-kura akan berputar 360 derajat), error sudutnya harus melalui rumus normalisasi `angle = atans2(sin(angle), cos(e)`
"PID nya dimana" anda mungkin bertanya. 
Program ini berperan sebagai publisher untuk topik `/turtle1/cmd_vel` sekaligus subscriber topik `/turtle1/pose via posCallback()`. Artinya tiap kali posisi kura-kura berpindah, ia akan merekalkulasi perintah kecepatan untuk mencapai waypoint dengan akurat. 

### pen up pen down pen up pen down pen up pen down pen up...
<img width="406" height="432" alt="image" src="https://github.com/user-attachments/assets/80c5a089-8352-43a3-a1bb-7ba15dc5099a" />

masalah yang jelas langsung muncul adalah ketika turtle berjalan dari origin ke titik pertama, dia akan mencoreng garis yang JELEK BET. hal ini juga menyebabkan kita tidak bisa menggambar bidang yang terpisah dari bidang utama. maka solusinya adalah service `SetPen` bawaan turtlesim.
```cpp
void setPen(bool enabled) {
        if (!pen->wait_for_service(std::chrono::seconds(1))) return;
        auto request = std::make_shared<turtlesim::srv::SetPen::Request>();
        request->r = 255;
        request->g = 255;
        request->b = 255;
        request->width = 3;
        request->off = enabled ? 0 : 1; 
        pen->async_send_request(request);
    }

...

void posCallback(turtlesim::msg::Pose::SharedPtr msg) {
        ...
        }
        else {
            if(activatePen) {
                setPen(1);
                activatePen = 0;
            }
            waypointIdx++;

            if(waypointIdx < waypoints.size() && waypoints[waypointIdx].first < 0.0) {
                setPen(0); 
                waypointIdx++;
                activatePen = 1;
            }
            ...
        } 
        
    }
```
disini, setiap kali kita mendapati nilai -1 pada waypoint, kita akan anggap itu sebagai perintah untuk mematikan pena. lalu iterasikan idx dan berjalan ke waypoint selanjutnya, dimana ia akan menerima flag `activatePen` yang sebelumnya diberi nilai 1 untuk menyalakan pen kembali sebelum lanjut bergerak ke titik berikutnya lagi.

### YAUDAH SIH GITU AJA BENTAR YA MAS HABIS INI AKU PUSH LAGI BUAT LINK YOUTUBE NYA


#include<SPI.h>            // thư viện để kết nối shield
#include<Ethernet.h>       // thư viện cho enthernet shield
#include <SoftwareSerial.h> // thư viện giao tiếp serial
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };   // địa chỉ MAC( địa chỉ phần cứng của shield ethernet)
IPAddress ip(192, 168, 0, 2);                         // địa chỉ IP của shield

EthernetServer server = EthernetServer(80);           // giá trị port là 80
String readString;                                   

const int DEV1 = 3;
const int DEV2 = 5;
const int DEV3 = 6;
const int DEV4 = 7;

// Init cho việc Nhận uid từ arduino số 2
SoftwareSerial rfSerial(8,9); //Tx, Rx cho arduino 2
String uid = ""; // UID do arduino 2 trả về 
unsigned long timeout = 10000;
unsigned long timee = 0;
void setup() {

  Serial.begin(9600);
  rfSerial.begin(9600);
                       
  while (!Serial || !rfSerial) {
    ;
  }
  pinMode(DEV1, OUTPUT);
  pinMode(DEV2, OUTPUT);
  pinMode(DEV3, OUTPUT);
  pinMode(DEV4, OUTPUT);
  digitalWrite(DEV1, LOW);
  digitalWrite(DEV2, LOW);
  digitalWrite(DEV3, LOW);
  digitalWrite(DEV4, LOW);
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("Dia chi server la: ");
  Serial.println(Ethernet.localIP());
  Serial.println("Ban da ket noi thanh cong mang ethernet...");

}

void loop() {
  EthernetClient client = server.available();           
  delay(2000);
  
  if (client = true) { //if client:

    while (client.connected()) { 
      if (client.available()) {
        char c = client.read();               //Đọc char bởi yêu cầu HTTP
        
        if (readString.length() < 100) {     //Lưu trữ các ký tự vào chuỗi
          
          readString += c;
        }
        
        if (c == '\n') {
          Serial.println(readString);                //In ra serial monitor
          client.println("HTTP/1.1 200 OK");        //Tạo 1 trang mới
          client.println("Content-Type: text/html");
          client.println("Refresh: 5");             // lệnh để tự động tải lại trang, cứ 5 giây 1 lần tải lại.
          client.println();
          client.println("<HTML>");
          client.println("<HEAD>");
          client.println("<TITLE>Ethernet Shield Arduino</TITLE>");
          
          client.println("</HEAD>");
          client.println("<BODY>");
          client.println("<CENTER>");
          client.println("<h1>HELLO BABY</h1>");
          client.println("<h3>************************************************************</h3>");
          //===========================================================================
          client.println("<br/>");                                        // lệnh để cách xuống dòng   
          client.println("<a href='button1on'> DEV 1 ON</a>");        // tạo một nút (button) trên web( nút bật thiết bị 1), khi bấm nút tức là ta đã gửi 1 lệnh về shield( arduino sẽ sử lí tín hiệu này)
          client.println("<br/>");
          client.println("<a href='button1off'>DEV 1 OFF</a>");      // nút dùng để tắt thiết bị 1
          client.println("<br/>");

          client.println("<br/>");
          client.println("<a href='button2on'>DEV 2 ON</a>");       // tương tự nút bật trên nhưng là tb 2
          client.println("<br/>");
          client.println("<a href='button2off'>DEV 2 OFF</a>");     // tương tự nút tắt trên nhưng là thiết bị 2
          client.println("<br/>");

          client.println("<br/>");
          client.println("<a href='button3on'>DEV 3 ON</a>");  
          client.println("<br/>");
          client.println("<a href='button3off'>DEV 3 OFF</a>");    
          client.println("<br/>");

          client.println("<br/>");
          client.println("<a href='button4on'>DEV 4 ON</a>");  
          client.println("<br/>");
          client.println("<a href='button4off'>DEV 4 OFF</a>");    
          client.println("<br/>");

          client.println("<br/>");
          client.print(" UID:    ");                // in uid ra màn hình
          client.print(uid);                                           
          client.println("<br/>");    
          
          client.println("<br/>");
          client.println("</BODY>");
          client.println("</HTML>");
          delay(1);
          //Stop Client:
          client.stop();
          Serial.println("client disconnected");
           
          if (readString.indexOf("button1on") > 0) {     // nút bấm thiết bị 1( button1) được bấm thì
            digitalWrite(DEV1, HIGH);                       // xuất ra tín hiệu điện dương ra cổng 9
          }                                              // các phần dưới hiểu tương tự
          if (readString.indexOf("button1off") > 0) {
            digitalWrite(DEV1 , LOW);
          }
          
          if (readString.indexOf("button2on") > 0) {
            digitalWrite(DEV2, HIGH);
          }
          if (readString.indexOf("button2off") > 0) {
            digitalWrite(DEV2 , LOW);
          }

            if (readString.indexOf("button3on") > 0) {
            digitalWrite(DEV3, HIGH);
          }
          if (readString.indexOf("button3off") > 0) {
            digitalWrite(DEV3 , LOW);

          }
            if (readString.indexOf("button4on") > 0) {
            digitalWrite(DEV4, HIGH);
          }
          if (readString.indexOf("button4off") > 0) {
            digitalWrite(DEV4 , LOW);
          }
               
          readString = "";                       // đặt lại chuỗi thành không có giá trị nào
        }
        
      } 
    } 
  }  
  //Nhận tín hiệu uid từ arduino 2
  while (rfSerial.available() > 0) {
    timee = millis();
    char ch = rfSerial.read();
    uid += ch;
    Serial.write(ch);
  }
  if (( millis() - timee) > timeout)
    uid = "";
}

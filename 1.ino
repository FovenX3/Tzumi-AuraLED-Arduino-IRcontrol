#include <WiFi.h>
#include <WebServer.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

// WiFi 设置
const char* ssid = "wifissid";
const char* password = "password";

// 红外发送引脚
const uint16_t kIrLed = 4; // 例如GPIO 4
IRsend irsend(kIrLed);

// 创建Web服务器对象
WebServer server(80);

// 定义红外命令
const uint32_t irCommands[24] = {
  0xF700FF, 0xF7807F, 0xF740BF, 0xF7C03F, // 增亮, 减暗, 关灯, 开灯
  0xF720DF, 0xF7A05F, 0xF7609F, 0xF7E01F, // 红, 绿, 蓝, 白
  0xF710EF, 0xF7906F, 0xF750AF, 0xF7D02F, // 快闪
  0xF730CF, 0xF7B04F, 0xF7708F, 0xF7F00F, // 慢闪
  0xF708F7, 0xF78877, 0xF748B7, 0xF7C837, // 渐变
  0xF728D7, 0xF7A857, 0xF76897, 0xF7E817  // 呼吸灯
};

// 按钮名称
const char* buttonNames[24] = {
  "增亮0", "减暗1", "关灯2", "开灯3",
  "红4", "绿5", "蓝6", "白7",
  "8", "9", "10", "快闪11",
  "12", "13", "14", "慢闪15",
  "16", "17", "18", "渐变19",
  "20", "21", "22", "呼吸灯23"
};

void setup() {
  Serial.begin(115200);
  irsend.begin();
  
  // 连接WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("正在连接WiFi...");
  }
  Serial.println("WiFi连接成功!");
  Serial.println(WiFi.localIP());

  // 设置web页面
  server.on("/", []() {
    String html = "<!DOCTYPE html><html lang='zh-CN'><head><meta charset='UTF-8'><title>IR控制</title></head><body><h1>IR控制</h1><div style='display: flex; flex-wrap: wrap;'>";
    for (int i = 0; i < 24; i++) {
      html += "<button style='flex: 1 0 21%; margin: 5px; height: 50px;' onclick='sendCommand(" + String(i) + ")'>" + String(buttonNames[i]) + "</button>";
    }
    html += "</div><script>function sendCommand(code) { var xhttp = new XMLHttpRequest(); xhttp.open('GET', '/send?code=' + code, true); xhttp.send(); }</script></body></html>";
    server.send(200, "text/html", html);
  });

  // 处理发送红外命令
  server.on("/send", []() {
    if (server.hasArg("code")) {
      String codeStr = server.arg("code");
      int codeIndex = codeStr.toInt();
      if (codeIndex >= 0 && codeIndex < 24) {
        irsend.sendNEC(irCommands[codeIndex]);
        irsend.sendNEC(irCommands[codeIndex]);
        irsend.sendNEC(irCommands[codeIndex]);
        irsend.sendNEC(irCommands[codeIndex]);
        irsend.sendNEC(irCommands[codeIndex]);
        irsend.sendNEC(irCommands[codeIndex]);
        irsend.sendNEC(irCommands[codeIndex]);
        irsend.sendNEC(irCommands[codeIndex]);
        irsend.sendNEC(irCommands[codeIndex]);
        irsend.sendNEC(irCommands[codeIndex]);

        server.send(200, "text/plain", "success");
      } else {
        server.send(400, "text/plain", "无效的命令索引");
      }
    } else {
      server.send(400, "text/plain", "缺少命令参数");
    }
  });

  // 启动服务器
  server.begin();
}

void loop() {
  server.handleClient();
}

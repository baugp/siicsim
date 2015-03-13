#คู่มือติดตั้ง และใช้งาน SIC Simulator ฉบับภาษาไทย

# วิธีติดตั้ง #

  1. ดาวน์โหลดโค้ดโปรแกรม [siicsim.zip](http://siicsim.googlecode.com/files/siicsim.zip)
  1. แตกไฟล์ไปที่โฟลเดอร์ siicsim/
  1. เรียกคำสั่ง make ในโฟลเดอร์ siicsim/
  1. จะปรากฏ siicsim.exe

# วิธีใช้ #

## มันทำงานยังไง? ##
  * dev/ เป็นโฟลเดอร์ที่เก็บ device ต่างๆ (สำหรับใช้กับคำสั่ง RD และ WD)
  * ไฟล์ dev/00 เป็นที่เก็บ bootstrap loader
  * CPU เริ่มทำงานโดย
    * โหลดข้อมูลใน dev/00 ใส่ memory เริ่มต้นที่ address 0000 (โหลดมา 128 bytes)
    * ให้ PC ชี้ที่ 0000 และเริ่มทำงาน
  * CPU จะหยุดทำงานเมื่อ
    * PC = FFFFFF
    * เราสามารถทำให้ CPU หยุดทำงานได้ด้วยคำสั่ง RSUB (ตอนแรก L=FFFFFF)

## เรียกใช้ siicsim.exe ##
```
./siicsim.exe
```
รันโปรแกรมจนกว่าจะหยุดทำงาน แล้วเซฟค่าใน Register และ Memory ลงไฟล์ memdump.txt

```
./siicsim.exe -s
```
รันโปรแกรมทีละคำสั่ง และแสดงผลการรันบนหน้าจอด้วย

```
./siicsim.exe -s -c
```
(สำหรับ Linux เท่านั้น) เหมือนกับ -s เฉยๆ แต่มีสีด้วย ว่า Memory ตรงไหนเปลี่ยนแปลง

## ตอนนี้มีอะไรใน dev/00 ##
ถ้าลองเปิดไฟล์ dev/00 ดูจะเจอ
```
0000090c000c4c0000123456
```
ต้นฉบับโปรแกรมจริงๆ คือ
```
Address Machine
0000    000009    LDA 0009 โหลดข้อมูลจาก 0009 ใส่ A
0003    0c000c    STA 000c เซฟข้อมูลจาก A ใส่ 000C
0006    4c0000    RSUB
0009    123456   (ค่าคงที่)
```
เป็นตัวอย่างโปรแกรมเฉยๆ ครับ
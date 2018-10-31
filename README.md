# Convertor de protocoale - implementare HW si SW

- "Communication protocol converter" se refera la proiectarea si realizarea unei placi electronice dotate cu un microcontroller, care sa aiba mai multe interfete de comunicatie seriala si sa realizeze functia de "gateway", adica sa preia datele transmise pe o interfata si sa le transmita mai departe pe alta interfata. Practic ar trebui ca orice combinatie sa fie posibila (da la CAN la LIN, de la I2C la RS232, de la RS232 la CAN , etc).

## Obiective:

1. Proiectarea schema electronice - microcontroller, transceivere (CAN, LIN, RS232, I2C), local power supply, butoane de selectie mod de lucru, display LCD (poate fi text pe 2 sau 4 linii, sau grafic), interfata cu PC-ul pe USB, interfata de programare ISP/JTAG

2. Proiectarea PCB-ului - 2 sau 4 straturi, in functie de complexitate. Respectarea regulilor de placement si rutare semnale de comunicatie

3. Proiectare firmware - programul ce va fi incarcat in microcontroller

4. Optional - proiectare aplicatie pe PC care sa trimita comenzi placii cu microcontroller. (Altfel controlul se poate realiza din butoane si se poate verifica modul de lucru pe display). Interfata cu PC-ul este pe USB.
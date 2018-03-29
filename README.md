
## Light Detector

https://youtu.be/AEo-e3eQIuw

Celem poniższego projektu było opracowanie prostego czujnika określającego położenie i odległość do źródła światła z wykorzystaniem modułu ADC mikrokontrolera Atmega32.

Czujnik przekazuje dane na wyświetlacz LCD 2x16 w postaci komunikatów: LEFT, FRONT, RIGHT oraz znaczników odległości ">". Czujnik wykrywa źródło światła w promieniu do około 50 cm.

Czujnik składa się z dwóch fotorezystorów opisanych jako FL (fotorezystor lewy), FP (fotorezystor prawy) oraz rezystorów i potencjometrów tworzących dzielniki napięcia dla pomiaru ADC. Światłoczułe powierzchnie fotorezystorów usytuowane są względem siebie pod kątem około 90 stopni. 
Na schemacie przedstawiono układ dla jednego fotorezystora.



![atmega adc poprawiony](https://user-images.githubusercontent.com/36235060/38105326-0984c864-338c-11e8-859e-9152c0a26317.PNG)

Vcc = 4,76 V 

R1= 1 kΩ 

R2= 10 kΩ 

R3= 10kΩ max. 

Fl, FP = 8kΩ max.


Zadanie mikrokontrolera polega na zmierzeniu napięcia za fotorezystorem w punkcie A. 
Wraz ze wzrostem natężenia światła padającego na fotorezystor jego rezystancja maleje. W celu ustalenia maksymalnego napięcia mogącego pojawić się w punkcie A zbliżyłem źródło światła (białą lampkę led telefonu) na odległość około 1 cm zmierzyłem wynik dla obu fotorezystorów. Zestawienie poniżej.


![tab1](https://user-images.githubusercontent.com/36235060/38105224-c4cf8b6e-338b-11e8-9b92-3cb10fa981da.PNG)

Napięcie referencyjne modułu ADC zostało ustawione programowo na Vref 2,56 V wg noty katalogowej. Rzeczywiste, zmierzone napięcie referencyjne wyniosło  Vref . zmierzone= 2,55 V.

W celu dokonania poprawnego pomiaru konieczne było zastosowanie dzielnika napięcia tak by w punkcie B  dla maksymalnego napięcia VA max. pojawiło się  napięcie  V ref . Zmierzone.  Wartość rezystora R2 ustawiono  na 10 kΩ oraz odpowiednio dostrojono potencjometr R3. 

# Kalibracja


W trakcie próbnych pomiarów okazało się, że wartość zmiennej difference (różnica wartości ADC obu fotorezystorów) dla źródła światła ustawionego nad fotorezystorami znacznie odbiegała od 0. Co mogło fałszować określanie pozycji śródła światła.

W celu zniwelowania tego błędu dokonano kilku pomiarów dla różnych odległości i natężeń światła. 
Testowe źródło światła ustawiano możliwie jak najbliżej środkowej pozycji. ( z wyjątkiem rozproszonego światła dziennego) Zestawienie poniżej.

![tab2](https://user-images.githubusercontent.com/36235060/38105291-f2c8d746-338b-11e8-8771-6c13c2de95c9.PNG)

Wartość średnia = 1,16

Oznacza to że FP jest bardziej światłoczuły niż FL. Może to wynikać z niedokładnego ustawienia fotorezystorów oraz ich budowy.
W sytuacji idealnej równałoby się 1. W celu poprawy odczytów dokonano programowej korekty odczytu FP, odpowiada za to linia kodu:

int16_t right_fotoresistor_value_correct =right_fotoresistor_value-(right_fotoresistor_value*0.16);

Reszta opisu znajduje się w komentarzach w  kodzie programu.

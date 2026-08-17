Odyssey Crew - README

1) Co robi

* Ekran wyboru inwestycji — lista budowana z bazy (nie zakodowana na sztywno).
* Budynki generowane proceduralnie jako prostopadłościany, układ wyliczany z danych.
* Kolorowanie mieszkań według statusu sprzedaży (kolory pobierane z bazy).
* Panel szczegółów lokalu po kliknięciu (nazwa, status, metraż, cena, dodatki).
* Nawigacja między budynkami — kamera przesuwa się „po szynach" wzdłuż osi X.

2) Architektura

Zasada nadrzędna: wyraźny rozdział warstwy dostępu do danych od warstwy prezentacji. Tylko jedna klasa w całym projekcie wie o istnieniu SQLite; reszta operuje na czystych strukturach.

UOdysseyDatabaseSubsystem (GameInstanceSubsystem) — jedyne miejsce znające SQLite. Otwiera bazę read-only, wykonuje zapytania i zwraca gotowe USTRUCT-y (FOdysseyInvestment, FOdysseyBuilding, FOdysseyFlat, FOdysseyFlatStatus, FOdysseyExtra). Widoczność (active) oraz konwersje (grosze, wartości NULL) są zaszyte w zapytaniach, więc warstwa wyżej dostaje dane już „przetłumaczone".

Warstwa prezentacji
AOdysseyBuildingsActor — generuje geometrię budynków z danych i koloruje mieszkania. Nie dotyka SQLite; pyta subsystem o struktury.
AOdysseyPlayerController — obsługuje inputy użytkownika, wybór mieszkania trace'em, obsługa wyboru inwestycji i nawigacji między budynkami.
AOdysseyHUD — tworzy i dodaje do viewportu główny widget.
UOdysseyMainUserWidget — kontener na pod-widgety (przez BindWidget).
UOdysseyFlatDetailsWidget — panel szczegółów; formatuje dane do wyświetlenia.
UOdysseyInvestmentSelectWidget + UOdysseyInvestmentEntryWidget — ekran wyboru inwestycji (lista budowana z danych).
UOdysseyChangeBuildingWidget — przyciski przełączania budynku.

Przepływ zależności jest jednokierunkowy: dane → prezentacja. Widget nigdy nie sięga do bazy; widgety tylko zgłaszają zdarzenia (np. „wybrano inwestycję").

3) Decyzje techniczne i ich uzasadnienie

Subsystem jako główna baza informacji. SQLite jest zamknięte w jednym miejscu, żeby reszta kodu była niezależna od źródła danych i testowalna. Gdyby dane przyszły kiedyś z API zamiast z pliku, zmienia się tylko subsystem — struktury i cała prezentacja zostają.

Wydajność — jeden ISM (Instanced Static Mesh) i per-instance custom data. Wszystkie mieszkania całej inwestycji trafiają do jednego InstancedStaticMeshComponent na współdzielonym sześcianie. Daje to zasadniczo jeden draw call na cały widok niezależnie od liczby lokali, zamiast setek osobnych aktorów. Kolor statusu przekazuję przez per-instance custom data (kanały 0–2 to RGB, kanał 3 to podświetlenie zaznaczenia), więc nie tworzę dynamicznych instancji materiału per bryła. Geometria budowana jest raz — przy wyborze inwestycji — i nie ma żadnej pracy per klatka. Kolizja ISM jest ustawiona na QueryOnly wyłącznie po to, żeby działał trace przy klikaniu.

Aplikacja tylko czyta, więc tryb read-only eliminuje ryzyko blokady pliku i przypadkowego zapisu. EnsureOpen() otwiera bazę z domyślnej ścieżki, jeśli jeszcze nie jest otwarta — dzięki temu kolejność BeginPlay różnych klas (aktor, kontroler, widgety) przestaje mieć znaczenie, bo każdy konsument najpierw upewnia się, że baza jest gotowa.

Liczba kondygnacji bierze się z floors_count, liczba i rozłożenie mieszkań na piętrze — z faktycznych rekordów, a szerokość bryły jest proporcjonalna do area, żeby na oko było widać różnicę między kawalerką a lokalem 120 m². Mieszkania dwupoziomowe (duplexy) renderuję jako bryły o podwójnej wysokości. Ponieważ baza nie zawiera pozycji X/Y mieszkań, układ poziomy jest z konieczności schematyczny (wymyślony).

MainWidget służy za kontener spinający pod-widgety.

4) Nietypowości zauważone w danych

a) Ukryty budynek pociąga za sobą swoje mieszkania. Budynek E (buildings.id = 5) ma active = 0, mimo że wszystkie jego 42 mieszkania mają active = 1. Dlatego widoczność jest sprawdzana kaskadowo: inwestycja i budynek i mieszkanie muszą być aktywne (JOIN w zapytaniu o mieszkania).

b) Pojedyncze wycofane mieszkania. Niezależnie od budynku, active = 0 mają też lokale A.1.2, A.2.1, A.2.2 i B.1.2 — nie trafiają do prezentacji ani do geometrii.

c) Ceny są w groszach, nie w złotówkach. price i price_sqm to liczby całkowite w groszach (1 zł = 100). Wyświetlane raw wyglądałyby jak miliony — dzielę przez 100 aby otrzymać cene w zł.

d) NULL w cenie to nie zero i nie „sprzedane". price = NULL oznacza „brak ceny do publicznej prezentacji" → pokazuję „Cena na zapytanie". W strukturze rozróżniam „brak ceny" od „cena = 0" osobną flagą. Co istotne, NULL w cenie występuje też przy statusie available, więc statusu nie wolno wnioskować z obecności ceny.

e) Mieszkania dwupoziomowe (duplexy). Gdy floor > entry_floor (o jeden), lokal zajmuje dwie kondygnacje — renderowany jest jako bryła podwójnej wysokości, z rezerwacją kolumny na piętrze wyżej.

f) mesh_id bywa NULL. Kolumna miała wskazywać gotową bryłę w scenie 3D. W tym zadaniu geometrię generuję z danych, więc pole jest w zasadzie informacyjne i świadomie je pomijam.

g) Id budynków nie są ciągłe w obrębie inwestycji. Inwestycja 1 ma budynki o id 1, 2, 3, ale inwestycja 2 — budynek o id 4 (bo id 1–3 zajęły A/B/C, a E jest ukryte). Dlatego pozycję budynku w kadrze (np. do nawigacji kamery) wyliczam z kolejności iteracji, a nie z id.

h) Kolejność i kolory pochodzą z bazy. Budynki sortuję po order_number (nie po id), a kolory statusów biorę z flat_statuses.color_hex zamiast kodować własne. Waluta jest czytana z investments.currency (w danych zawsze PLN, ale nie zakładam tego na sztywno).

5) Znane uproszczenia i możliwe rozwinięcia

Poziomy układ mieszkań jest schematyczny — nie odwzorowuje realnego rzutu, bo danych o pozycji X/Y w bazie nie ma.
Przy duplexie kolumna nad nim jest rezerwowana, co bywa widoczne jako wąska przerwa — to celowe.
Przy znacznie większej skali danych warto by dołożyć doczytywanie dodatków jednym zapytaniem na budynek zamiast per mieszkanie oraz rozważyć LOD; przy obecnych 164 lokalach nie ma to znaczenia, a instancing (ISM) i tak utrzymuje liczbę draw calli na minimum.

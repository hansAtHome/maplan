(define (problem thoughtful-s13-t7)
(:domain thoughtful-typed)
(:objects
    C0 CA C2 C3 C4 C5 C6 C7 C8 C9 CT CJ CQ CK
    D0 DA D2 D3 D4 D5 D6 D7 D8 D9 DT DJ DQ DK
    H0 HA H2 H3 H4 H5 H6 H7 H8 H9 HT HJ HQ HK
    S0 SA S2 S3 S4 S5 S6 S7 S8 S9 ST SJ SQ SK
 - card
    COLN0 COLN1 COLN2 COLN3 COLN4 COLN5 COLN6 COLN7
 - colnum
    N0 N1 N2 N3 N4 N5 N6 N7 N8 N9 N10 N11 N12 N13
 - num
    C D H S
 - suit
)
(:init
(VALUE C0 N0)
(VALUE D0 N0)
(VALUE H0 N0)
(VALUE S0 N0)
(VALUE CA N1)
(VALUE DA N1)
(VALUE HA N1)
(VALUE SA N1)
(VALUE C2 N2)
(VALUE D2 N2)
(VALUE H2 N2)
(VALUE S2 N2)
(VALUE C3 N3)
(VALUE D3 N3)
(VALUE H3 N3)
(VALUE S3 N3)
(VALUE C4 N4)
(VALUE D4 N4)
(VALUE H4 N4)
(VALUE S4 N4)
(VALUE C5 N5)
(VALUE D5 N5)
(VALUE H5 N5)
(VALUE S5 N5)
(VALUE C6 N6)
(VALUE D6 N6)
(VALUE H6 N6)
(VALUE S6 N6)
(VALUE C7 N7)
(VALUE D7 N7)
(VALUE H7 N7)
(VALUE S7 N7)
(VALUE C8 N8)
(VALUE D8 N8)
(VALUE H8 N8)
(VALUE S8 N8)
(VALUE C9 N9)
(VALUE D9 N9)
(VALUE H9 N9)
(VALUE S9 N9)
(VALUE CT N10)
(VALUE DT N10)
(VALUE HT N10)
(VALUE ST N10)
(VALUE CJ N11)
(VALUE DJ N11)
(VALUE HJ N11)
(VALUE SJ N11)
(VALUE CQ N12)
(VALUE DQ N12)
(VALUE HQ N12)
(VALUE SQ N12)
(VALUE CK N13)
(VALUE DK N13)
(VALUE HK N13)
(VALUE SK N13)
(COLSUCCESSOR COLN1 COLN0)
(COLSUCCESSOR COLN2 COLN1)
(COLSUCCESSOR COLN3 COLN2)
(COLSUCCESSOR COLN4 COLN3)
(COLSUCCESSOR COLN5 COLN4)
(COLSUCCESSOR COLN6 COLN5)
(COLSUCCESSOR COLN7 COLN6)
(SUCCESSOR N1 N0)
(SUCCESSOR N2 N1)
(SUCCESSOR N3 N2)
(SUCCESSOR N4 N3)
(SUCCESSOR N5 N4)
(SUCCESSOR N6 N5)
(SUCCESSOR N7 N6)
(SUCCESSOR N8 N7)
(SUCCESSOR N9 N8)
(SUCCESSOR N10 N9)
(SUCCESSOR N11 N10)
(SUCCESSOR N12 N11)
(SUCCESSOR N13 N12)
(SUITP C0 C)
(SUITP D0 D)
(SUITP H0 H)
(SUITP S0 S)
(SUITP CA C)
(SUITP DA D)
(SUITP HA H)
(SUITP SA S)
(SUITP C2 C)
(SUITP D2 D)
(SUITP H2 H)
(SUITP S2 S)
(SUITP C3 C)
(SUITP D3 D)
(SUITP H3 H)
(SUITP S3 S)
(SUITP C4 C)
(SUITP D4 D)
(SUITP H4 H)
(SUITP S4 S)
(SUITP C5 C)
(SUITP D5 D)
(SUITP H5 H)
(SUITP S5 S)
(SUITP C6 C)
(SUITP D6 D)
(SUITP H6 H)
(SUITP S6 S)
(SUITP C7 C)
(SUITP D7 D)
(SUITP H7 H)
(SUITP S7 S)
(SUITP C8 C)
(SUITP D8 D)
(SUITP H8 H)
(SUITP S8 S)
(SUITP C9 C)
(SUITP D9 D)
(SUITP H9 H)
(SUITP S9 S)
(SUITP CT C)
(SUITP DT D)
(SUITP HT H)
(SUITP ST S)
(SUITP CJ C)
(SUITP DJ D)
(SUITP HJ H)
(SUITP SJ S)
(SUITP CQ C)
(SUITP DQ D)
(SUITP HQ H)
(SUITP SQ S)
(SUITP CK C)
(SUITP DK D)
(SUITP HK H)
(SUITP SK S)
(KING CK)
(KING DK)
(KING HK)
(KING SK)
(CANSTACK C2 D3)
(CANSTACK C2 H3)
(CANSTACK S2 D3)
(CANSTACK S2 H3)
(CANSTACK D2 C3)
(CANSTACK D2 S3)
(CANSTACK H2 C3)
(CANSTACK H2 S3)
(CANSTACK C3 D4)
(CANSTACK C3 H4)
(CANSTACK S3 D4)
(CANSTACK S3 H4)
(CANSTACK D3 C4)
(CANSTACK D3 S4)
(CANSTACK H3 C4)
(CANSTACK H3 S4)
(CANSTACK C4 D5)
(CANSTACK C4 H5)
(CANSTACK S4 D5)
(CANSTACK S4 H5)
(CANSTACK D4 C5)
(CANSTACK D4 S5)
(CANSTACK H4 C5)
(CANSTACK H4 S5)
(CANSTACK C5 D6)
(CANSTACK C5 H6)
(CANSTACK S5 D6)
(CANSTACK S5 H6)
(CANSTACK D5 C6)
(CANSTACK D5 S6)
(CANSTACK H5 C6)
(CANSTACK H5 S6)
(CANSTACK C6 D7)
(CANSTACK C6 H7)
(CANSTACK S6 D7)
(CANSTACK S6 H7)
(CANSTACK D6 C7)
(CANSTACK D6 S7)
(CANSTACK H6 C7)
(CANSTACK H6 S7)
(CANSTACK C7 D8)
(CANSTACK C7 H8)
(CANSTACK S7 D8)
(CANSTACK S7 H8)
(CANSTACK D7 C8)
(CANSTACK D7 S8)
(CANSTACK H7 C8)
(CANSTACK H7 S8)
(CANSTACK C8 D9)
(CANSTACK C8 H9)
(CANSTACK S8 D9)
(CANSTACK S8 H9)
(CANSTACK D8 C9)
(CANSTACK D8 S9)
(CANSTACK H8 C9)
(CANSTACK H8 S9)
(CANSTACK C9 DT)
(CANSTACK C9 HT)
(CANSTACK S9 DT)
(CANSTACK S9 HT)
(CANSTACK D9 CT)
(CANSTACK D9 ST)
(CANSTACK H9 CT)
(CANSTACK H9 ST)
(CANSTACK CT DJ)
(CANSTACK CT HJ)
(CANSTACK ST DJ)
(CANSTACK ST HJ)
(CANSTACK DT CJ)
(CANSTACK DT SJ)
(CANSTACK HT CJ)
(CANSTACK HT SJ)
(CANSTACK CJ DQ)
(CANSTACK CJ HQ)
(CANSTACK SJ DQ)
(CANSTACK SJ HQ)
(CANSTACK DJ CQ)
(CANSTACK DJ SQ)
(CANSTACK HJ CQ)
(CANSTACK HJ SQ)
(CANSTACK CQ DK)
(CANSTACK CQ HK)
(CANSTACK SQ DK)
(CANSTACK SQ HK)
(CANSTACK DQ CK)
(CANSTACK DQ SK)
(CANSTACK HQ CK)
(CANSTACK HQ SK)
(COLSPACE COLN0)
(BOTTOMCOL CQ)
(CLEAR CQ)
(FACEUP CQ)
(BOTTOMCOL S6)
(ON S3 S6)
(CLEAR S3)
(FACEUP S3)
(BOTTOMCOL H2)
(ON ST H2)
(ON SQ ST)
(CLEAR SQ)
(FACEUP SQ)
(BOTTOMCOL SK)
(ON CJ SK)
(ON HA CJ)
(ON DT HA)
(CLEAR DT)
(FACEUP DT)
(BOTTOMCOL HQ)
(ON H9 HQ)
(ON C7 H9)
(ON CK C7)
(ON H4 CK)
(CLEAR H4)
(FACEUP H4)
(BOTTOMCOL SJ)
(ON C4 SJ)
(ON HK C4)
(ON DQ HK)
(ON S2 DQ)
(ON D6 S2)
(CLEAR D6)
(FACEUP D6)
(BOTTOMCOL H6)
(ON C2 H6)
(ON S7 C2)
(ON C5 S7)
(ON C6 C5)
(ON C8 C6)
(ON S8 C8)
(CLEAR S8)
(FACEUP S8)
(BOTTOMTALON D3)
(ONTALON H7 D3)
(ONTALON DA H7)
(ONTALON D9 DA)
(ONTALON H3 D9)
(ONTALON D2 H3)
(ONTALON D7 D2)
(ONTALON CA D7)
(ONTALON D8 CA)
(ONTALON S5 D8)
(ONTALON S4 S5)
(ONTALON D4 S4)
(ONTALON C9 D4)
(ONTALON H8 C9)
(ONTALON S9 H8)
(ONTALON C3 S9)
(ONTALON DJ C3)
(ONTALON H5 DJ)
(ONTALON HT H5)
(ONTALON CT HT)
(ONTALON HJ CT)
(ONTALON SA HJ)
(ONTALON DK SA)
(ONTALON D5 DK)
(TOPTALON D5)
(TALONPLAYABLE DA)
(HOME C0)
(HOME D0)
(HOME H0)
(HOME S0)
)
(:goal
(and
(HOME CK)
(HOME DK)
(HOME HK)
(HOME SK)
)
)
)


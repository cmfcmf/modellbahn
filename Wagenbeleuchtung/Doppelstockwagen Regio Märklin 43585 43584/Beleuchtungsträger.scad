laenge = 149;
stegLaenge = 13.5;
stegBreite = 1.2;
stegFreiraum = 2;
dicke = 1.4;
breite = 8;

kabelkanal = 1.5;

ende_laenge = 35;
ende_support_laenge = 5;


module streifen() {
    translate([stegFreiraum, -breite / 2, -0.2])
    cube([laenge + 2 * (stegLaenge - stegFreiraum), breite, dicke]);
}

module support() {
    support_breite = breite - 2 * kabelkanal;
    support_dicke = 4;

    color("green")
    translate([-ende_support_laenge - stegFreiraum, -support_breite / 2, -support_dicke])
    cube([stegLaenge + ende_support_laenge + stegFreiraum, support_breite, support_dicke]);

    translate([-ende_laenge - stegFreiraum, -breite / 2, -0.2])
    cube([ende_laenge, breite, dicke]);
}

module steg() {
    color("red")
    translate([0, -stegBreite / 2, -30 + 0.4])
    cube([stegLaenge, stegBreite, 30]);
}


difference() {
    union() {
        support();
        streifen();
        translate([laenge + 2 * (stegLaenge), 0, 0])
        rotate(180)
        support();
    };
    steg();
    translate([laenge + 2 * (stegLaenge), 0, 0])
    rotate(180)
    steg();
}
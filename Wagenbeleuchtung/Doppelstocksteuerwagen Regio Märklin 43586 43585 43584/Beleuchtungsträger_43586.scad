laenge = 220 + 3;
dicke = 1.4;
breite = 8;

// länge bis 1. pfosten: 91mm
// länge zwischen pfosten 34mm

// 46mm höhe umschaltplatine
// 51mm höhe sützen
// 42mm höhe hinten
// 49mm höhe bei den außenstützen

stuetzeLaenge = 1.4;
stuetzeBreite = 2.5; // 2.3;
stuetzeAbstand = 10;


brueckeBreite = 15;
brueckeLaenge = 4;
brueckeTiefe = 2;


module streifen() {
    translate([0, -breite / 2, 0])
    cube([laenge, breite, dicke]);
}

module stuetze() {
    translate([-stuetzeLaenge/2, -stuetzeBreite/2, -brueckeTiefe])
    cube([stuetzeLaenge, stuetzeBreite, brueckeTiefe]);
}

module bruecke() {
    difference() {
        color("green")
        translate([-brueckeLaenge/2, -brueckeBreite/2, -brueckeTiefe])
        cube([brueckeLaenge, brueckeBreite, dicke + brueckeTiefe]);
    
        translate([0, stuetzeAbstand/2, 0])
        stuetze();
        translate([0, -stuetzeAbstand/2, 0])
        stuetze();
        translate([-brueckeLaenge/2, -breite/2, -brueckeTiefe])
        cube([brueckeLaenge, breite, dicke + brueckeTiefe]);
    }
}

module auflage() {
    auflageDicke = 51 - 46;
    color("red")
    translate([0, -breite / 2, -auflageDicke])
    cube([3, breite, auflageDicke]);
}

module auflageHinten() {
    auflageLaenge = 5;
    auflageDicke = 51 - 42;
    auflageBreite = 3;
    breiteOffset = 1;
    color("orange")
    translate([laenge - auflageLaenge, -breite / 2 - auflageBreite - breiteOffset, -auflageDicke])
    union() {
        cube([auflageLaenge, auflageBreite, auflageDicke]);
        translate([0, 0, auflageDicke])
        prism(auflageLaenge, auflageBreite + breiteOffset, dicke);
    }
    color("orange")
    translate([laenge - auflageLaenge, breite / 2 + breiteOffset, -auflageDicke])
    union() {
        cube([auflageLaenge, auflageBreite, auflageDicke]);
        translate([0, auflageBreite, auflageDicke])
        mirror([0, 1, 0])
        prism(auflageLaenge, auflageBreite + breiteOffset, dicke);
    }
}

module auflageHintenAlt() {
    auflageLaenge = 5;
    auflageDicke = 51 - 42;
    auflageBreite = 3;
    breiteOffset = 1;
    color("red")
    translate([laenge - auflageLaenge, -breite / 2 - auflageBreite, -auflageDicke])
    cube([auflageLaenge, auflageBreite, auflageDicke + dicke]);
    color("red")
    translate([laenge - auflageLaenge, breite / 2, -auflageDicke])
    cube([auflageLaenge, auflageBreite, auflageDicke + dicke]);
}

module prism(l, w, h) {
    polyhedron(
        points=[[0,0,0], [l,0,0], [l,w,0], [0,w,0], [0,w,h], [l,w,h]],
        faces=[[0,1,2,3],[5,4,3,2],[0,4,5,1],[0,3,4],[5,2,1]]
    );
}

// intersection() {
//    union() {
        auflage();
        auflageHinten();
        // auflageHintenAlt();
        streifen();

        translate([91, 0, 0])
        bruecke();
        translate([91 + 34, 0, 0])
        bruecke();
//    };
//    translate([85, -40, -40])
//    cube([50, 1000, 10000]); 
// }
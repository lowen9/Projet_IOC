/* gestion switch led */

var led_switch = document.getElementById("led_switch");

var switch_val = document.getElementById("switch_val");

led_switch.addEventListener('change', function(){
    var led_val = this.checked ? 'ON' : 'OFF';
    console.log('Led:', led_val);
    switch_val.innerHTML = led_val;
    eel.writefile(led_val, "led", "database2esp.txt")();
});

/* recup le cube esp32 */

const cube = document.getElementById("cube_esp32");

var rotationAngleX = 0;
var rotationAngleY = 0;
var rotationAngleZ = 0;

// var switch_rotate_X = document.getElementById("rotateButtonX");
// var switch_rotate_Y = document.getElementById("rotateButtonY");
// var switch_rotate_Z = document.getElementById("rotateButtonZ");
// var reset_rotate = document.getElementById("resetButton");

// switch_rotate_X.addEventListener('click', function() {
//     rotationAngleX += 1; // Par exemple, tourne de 90 degrés à chaque clic
//     cube.style.transform = `rotateX(${rotationAngleX}deg) rotateY(${rotationAngleY}deg) rotateZ(${rotationAngleZ}deg)`;
// });

// switch_rotate_Y.addEventListener('click', function() {
//     rotationAngleY += 1; // Par exemple, tourne de 90 degrés à chaque clic
//     cube.style.transform = `rotateX(${rotationAngleX}deg) rotateY(${rotationAngleY}deg) rotateZ(${rotationAngleZ}deg)`;
// });

// switch_rotate_Z.addEventListener('click', function() {
//     rotationAngleZ += 1; // Par exemple, tourne de 90 degrés à chaque clic
//     cube.style.transform = `rotateX(${rotationAngleX}deg) rotateY(${rotationAngleY}deg) rotateZ(${rotationAngleZ}deg)`;
// });

// reset_rotate.addEventListener('click', function(){
//     rotationAngleX = 0;
//     rotationAngleY = 0;
//     rotationAngleZ = 0;
//     cube.style.transform = `rotateX(${rotationAngleX}deg) rotateY(${rotationAngleY}deg) rotateZ(${rotationAngleZ}deg)`;
// })


/* recup photoR value */
let balisePhotoR = document.getElementById("photoVal");

let progress_bar = document.getElementById("progress_value");

/* recup Boutuon value */
let balise_btn = document.getElementById("bouton_val");
let balise_btn_c = document.getElementById("bouton_c");
var bouton_val = 0;


/* Gestion de la base de donnée */

/* Lecture de la database et modification du site */
let data;

async function update_value(){
    const readdatabase = eel.readfile("database2web.txt");
    data = await readdatabase();
    //   console.log(data);

    balisePhotoR.innerHTML = data[0];
    progress_bar.style.width = data[0];
    rotationAngleX = data[1];
    rotationAngleY = data[2];
    rotationAngleZ = data[3];
    cube.style.transform = `rotateX(${rotationAngleX}deg) rotateY(${rotationAngleY}deg) rotateZ(${rotationAngleZ}deg)`;
    bouton_val = data[4];
    console.log('btn:', bouton_val);
    if(bouton_val === 1){
        balise_btn.innerHTML = "appuyé";
        balise_btn_c.style.backgroundColor = "green";
    }
    else{
        balise_btn.innerHTML = "pas appuyé";
        balise_btn_c.style.backgroundColor = "red";
    }
    
}

setInterval(update_value, 100);

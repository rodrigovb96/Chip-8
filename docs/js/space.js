//-------------------------------SPACE ANIMATION---------------------------

// https://github.com/CompactJS/remap
const remap = (value, min, max, targetMin, targetMax) => {
    return targetMin + ((value - min) / (max - min)) * (targetMax - targetMin);
};
// https://github.com/CompactJS/random
const random = (min, max) => {
    if (min === undefined) return Math.random();
    if (max === undefined) return Math.random() * min;
    if (min > max) return Math.random() * (min - max) + max;
    return Math.random() * (max - min) + min;
};

const Star = (context) => {
    let sx, sy, size;
    return {
        x: 0,
        y: 0,
        z: 0,
        color: "",
        speed: 2,
        updateOnScreen({ width, height }) {
            const relativeWidth = width + width / 2;
            const relativeHeight = height + height / 2;
            sx = remap(this.x / this.z, 0, 1, width / 2, relativeWidth);
            sy = remap(this.y / this.z, 0, 1, height / 2, relativeHeight);
            size = remap(this.z, -relativeWidth / 2, relativeWidth, 5, 0);
            this.z -= this.speed;
            if (this.z < 1) {
                this.z = random(relativeWidth);
            }
        },
        draw() {
            context.fillStyle = this.color;
            context.fillRect(sx, sy, size, size);
        },
        init({ x, y, z, color = "rgba(255,255,255,0.5)" }) {
            this.x = x;
            this.y = y;
            this.z = z;
            this.color = color;
        },
        outOfWindow({ width, height }) {
            return this.x < 0 || this.x > width || this.y < 0 || this.y > height;
        }
    };
};

let windowWidth = window.innerWidth;
let windowHeight = window.innerHeight;
let centerX = windowWidth / 2;
let centerY = windowHeight / 2;

const canvas = document.getElementById("canvas-space");
const context = canvas.getContext("2d");
canvas.width = windowWidth;
canvas.height = windowHeight;

const stars = [];
const starCount = (windowHeight * windowWidth) / 200;

for (let i = 0; i < starCount; i++) {
    stars.push(Star(context));
}

stars.forEach((s) =>
    s.init({
        x: random(-centerX, centerX * 2),
        y: random(-centerY, centerY * 2),
        z: random(-centerX, centerX * 2)
    })
);

const draw = () => {
    context.fillStyle = "rgba(0,0,0,0.5)";
    context.fillRect(0, 0, windowWidth, windowHeight);
    stars.forEach((s) => {
        s.updateOnScreen({ width: windowWidth, height: windowHeight });

        s.draw();
    });
    requestAnimationFrame(draw);
};

window.addEventListener("resize", () => {
    canvas.width = windowWidth = window.innerWidth;
    canvas.height = windowHeight = window.innerHeight;
    centerX = windowWidth / 2;
    centerY = windowHeight / 2;
});

draw();

//----------------------------- TYPING ANIMATION--------------------------------
var options = {
    strings: ['user@space ~$ initializing navigation systems'],
    typeSpeed: 40
};
var options2 = {
    strings: ['user@space ~$ Controls ready to take off'],
    startDelay: 4000,
    typeSpeed: 40
};
var options3 = {
    strings: ['user@space ~$ STRAT.', 'user@space ~$ START'],
    startDelay: 8000,
    typeSpeed: 40
};

var typed = new Typed('.element0', options);
var typed2 = new Typed('.element1', options2);
var typed3 = new Typed('.element2', options3);

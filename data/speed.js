
let box = document.querySelector('.box')

let tween = gsap.to(box, { x: window.innerWidth - box.offsetWidth, repeat: -1, yoyo: true, ease: 'none', duration: 10 })

let buttons = gsap.utils.toArray('button')

buttons.forEach( function(btn, i) {
  
  btn.addEventListener('click', function() {
    //tween.timeScale( btn.getAttribute('data-speed') )
    gsap.to(tween, { timeScale: btn.getAttribute('data-speed'), overwrite: "auto" })
  })
  
})

/* Support list */

$("#slist a").click(function(e){
   e.preventDefault();
   $(this).next('p').toggle(200);
});

/* Portfolio */

// filter items when filter link is clicked
$('#filters a').click(function(){
  var selector = $(this).attr('data-filter');
  $container.isotope({ filter: selector });
  return false;
});


/* home links */
$(".afeature, .clients").click(function(e){
   e.preventDefault();
   window.location = $(this).attr("url");
});
Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');
  }
);
var word2;
var defn;
// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
    console.log('Received message: '+ JSON.stringify(e.payload));
    console.log('check data ' + e.data[0]);
    word2 = e.data[0].toLowerCase();
  }                     
);
var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function getDefinition() {
  var url = "https://glosbe.com/gapi/translate?from=eng&dest=eng&format=json&phrase="+word2+"&pretty=true";
  xhrRequest(url, 'GET',
      function(responseText) {
        var json = JSON.parse(responseText);
        var results = json.tuc;
         if (results && results.length > 0 && results[0].meanings && results[0].meanings.length > 0) {
           // we have at least 1 definition
          var definitions = results[0].meanings;
           var defn = "";
           for(var i=0;i<5;i++) {
             defn += (definitions[i].text+".\n\n");
           }
          //var defn = definitions[0].text;
          console.log('Definition: '+defn);
        }
      }
   );
  
  var transactionId = Pebble.sendAppMessage( { '0': defn },
  function(e) {
    console.log('Successfully delivered message with transactionId='
      + e.data.transactionId);
  },
  function(e) {
    console.log('Unable to deliver message with transactionId='
      + e.data.transactionId
      + ' Error is: ' + e.error.message);
  }
);
}
// Pebble.addEventListener('ready',
//   function(e) {
//     console.log('PebbleKit JS ready.');
//     getDefinition();
//   }
// );
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage Received.');
    getDefinition();
  });


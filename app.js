/*
Declared in .env file:
  YAMAHA_IP
  PARTICLE_USER
  PARTICLE_PASSWORD
  PARTICLE_ID
*/

var assert = require('assert');
var env = require('node-env-file');
var util = require('util');
var EventEmitter = require('events').EventEmitter;
var YamahaAPI = require("yamaha-nodejs");
var Particle = require('spark');

// when stringified, cannot be larger than 63 characters
var settings = {
  "begin":"begin",
  "Receiver":{
    "power":["on","off"],
    "volume":"int:-500,0",
    "mute":["on","off"],
    "input":["Pandora"]
  },
  "end":"end"
};
var stringSettings = JSON.stringify(settings);
console.log('stringSettings: ', stringSettings);
console.log('settings arg length: ', stringSettings.length);

env(__dirname + '/.env');

var yamaha = new YamahaAPI(process.env.YAMAHA_IP);
var particleDevice = 0;

// yamaha.powerOn().done(function () {
//   console.log("powerOn");
//   yamaha.setMainInputTo("Pandora").done(function () {
//     console.log("Switched to Pandora");
//     // default volume
//     yamaha.setVolumeTo(-350).done(function(){
//       console.log("Set volume to -350");
//     }, function(err){
//       console.log('error setting receiver volume to -350: ', err);
//     });
//   }, function (err) {
//     console.log('error switching receiver to pandora: ', err)
//   });
// }, function (err) {
//   console.log('error turning the receiver on: ', err)
// });

console.log(process.env.PARTICLE_ID);

Particle.login({ username: process.env.PARTICLE_USER, password:  process.env.PARTICLE_PASSWORD}, function (err, body) {
  Particle.getDevice( process.env.PARTICLE_ID, function (err, device)
  {
    if(device != null && err == null)
    {
      particleDevice = device;
      // console.log(particleDevice);
     
      particleDevice.subscribe('received', function(data) {
        console.log("Event: " + data);
      });
      
      // chunk of the call request in parts
      var index = 0;
      for (var index = 0 ; index < stringSettings.length; index += 63)
      {
        var settingsData = stringSettings.substring(index, index+63);
        console.log(settingsData + '~' + settingsData.length);  
        device.callFunction('setupStruct', settingsData, function(err, data)
        {
          if (err) {
            console.log('An error occurred:', err);
          } else {
            console.log('Function called succesfully:', data);
          }
        });
      }
    }else{
      console.log('particle device could not be found');
    }
  });
});


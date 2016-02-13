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
  "Receiver": {
    "power": ["on", "off"],
    "volume": "-500,0,10", // min, max, increments
    "mute": ["on", "off"],
    "input": ["Pandora", "HDMI1"]
  }
};
var stringSettings = 'begin--' + JSON.stringify(settings) + '--end';
console.log('stringSettings: ', stringSettings);
console.log('settings arg length: ', stringSettings.length);

env(__dirname + '/.env');

var yamaha = new YamahaAPI(process.env.YAMAHA_IP);
var particleDevice = 0;

console.log(process.env.PARTICLE_ID);

Particle.login({ username: process.env.PARTICLE_USER, password: process.env.PARTICLE_PASSWORD }, function (err, body) {
  Particle.getDevice(process.env.PARTICLE_ID, function (err, device) {
    if (device != null && err == null) {
      particleDevice = device;
      
      particleDevice.subscribe('call', function(data){
        var dataSaved = data.data;
        var delimiterIndex = dataSaved.indexOf(":");
        var identifier = dataSaved.substring(0, delimiterIndex);
        if(identifier == "Volume")
        {
          var volumeLevel = dataSaved.substring(delimiterIndex+1);
          yamaha.setVolumeTo(parseInt(volumeLevel)).done(function(){
            console.log("Set volume to "+volumeLevel);
          }, function(err){
            console.log('error setting receiver volume: ', err);
          });
        }
        else if(identifier == "Power")
        {
          var value = dataSaved.substring(delimiterIndex+1);
          if(value == "on")
          {
            yamaha.powerOn().done(function(){
              console.log("Set volume to "+volumeLevel);
            }, function(err){
              console.log('error setting receiver volume: ', err);
            });
          }else{
            yamaha.powerOff().done(function(){
              console.log("Set volume to "+volumeLevel);
            }, function(err){
              console.log('error setting receiver volume: ', err);
            });
          }
        }
        else if(identifier == "Mute")
        {
          var value = dataSaved.substring(delimiterIndex+1);
          if(value == "on")
          {
            yamaha.muteOn().done(function(){
              console.log("Turned Mute On");
            }, function(err){
              console.log('error setting mute', err);
            });
          }else{
            yamaha.muteOff().done(function(){
              console.log("Turned Mute Off");
            }, function(err){
              console.log('error setting mute', err);
            });
          }
        }
        else if(identifier == "Controls")
        {
          var value = dataSaved.substring(delimiterIndex+1);
          if(value == "play")
          {
            yamaha.play().done(function(){
              console.log("set play");
            }, function(err){
              console.log('error:', err);
            });
          }else if(value == "pause"){
            yamaha.pause().done(function(){
              console.log("set pause");
            }, function(err){
              console.log('error:', err);
            });
          }else if(value == "skip"){
            yamaha.skip().done(function(){
              console.log("set skip");
            }, function(err){
              console.log('error:', err);
            });
          }
        }
        else if(identifier == "Input")
        {
          var value = dataSaved.substring(delimiterIndex+1);
          yamaha.setMainInputTo(value);
        }
      });
    } else {
      console.log('particle device could not be found');
    }
  });
});
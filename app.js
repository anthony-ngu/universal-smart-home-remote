var assert = require('assert');
var config = require('./config.json');
var util = require('util');
var EventEmitter = require('events').EventEmitter;
var YamahaAPI = require('./yamaha/yamaha.js');
var Wemo = require('./Wemo.js');
var Particle = require('spark');

var wemoDevice = new Wemo({
  "name":config.WEMO_DEVICE
});

var yamaha = new YamahaAPI(config.YAMAHA_IP);
var particleDevice = 0;

console.log(config.PARTICLE_ID);

Particle.login({ username: config.PARTICLE_USER, password: config.PARTICLE_PASSWORD }, function (err, body) {
  Particle.getDevice(config.PARTICLE_ID, function (err, device) {
    if (device != null && err == null) {
      particleDevice = device;
      
      particleDevice.subscribe('call', function(data){
        var dataSaved = data.data;
        var delimiterIndex = dataSaved.indexOf(":");
        var identifier = dataSaved.substring(0, delimiterIndex);
        
        switch(identifier)
        {
          case "Volume":
            var volumeLevel = dataSaved.substring(delimiterIndex+1);
            yamaha.setVolumeTo(parseInt(volumeLevel)).done(function(){
              console.log("Set volume to "+volumeLevel);
            }, function(err){
              console.log('error setting receiver volume: ', err);
            });
            break;
          case "Power":
            var value = dataSaved.substring(delimiterIndex+1);
            if(value == "on")
            {
              yamaha.powerOn().done(function(){
                console.log("Set p ower on");
              }, function(err){
                console.log('error turning receiver on: ', err);
              });
            }else{
              yamaha.powerOff().done(function(){
                console.log("Set power off");
              }, function(err){
                console.log('error turning receiver off: ', err);
              });
            }
            break;
          case "Mute":
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
            break;
          case "Controls":
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
            break;
          case "Input":
            var value = dataSaved.substring(delimiterIndex+1);
            yamaha.setMainInputTo(value);
            break;
          case "Wemo":
            var value = dataSaved.substring(delimiterIndex+1);
            if(wemoDevice !== 'undefined')
            {
              if(value == "on")
              {
                wemoDevice.Toggle({
                  "on/off/both":"on"
                });
              }else{
                wemoDevice.Toggle({
                  "on/off/both":"off"
                });
              }
            }
            break;
          default:
            console.log("could not match identifier "+identifier);
            break;
        }
      });
    } else {
      console.log('particle device could not be found');
    }
  });
});
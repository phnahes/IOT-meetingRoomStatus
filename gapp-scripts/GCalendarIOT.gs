function doGet(e) {
  var cal = CalendarApp.getCalendarById('**********');
  if (cal == undefined) {
    return ContentService.createTextOutput('No access to calendar! ');}
  else{
    if(checkForEventsToday(cal) == true){
      var retrievedEvents = getEvents(cal);
      if(retrievedEvents == ''){
        setMeetingStatus("0");
        return ContentService.createTextOutput('Events later: \n' + getAllEventsHappeningLater(cal));}
      else{
        setMeetingStatus("1");
        return ContentService.createTextOutput('Current events: \n' + getEvents(cal));}
        
    }
    else{
      setMeetingStatus("0");
      return ContentService.createTextOutput('No events scheduled \n');}
  }
 }

function checkForEventsToday(cal){
  var now = new Date();
  var dayEnd = new Date();
  dayEnd.setHours(23,59,59);
  var events = cal.getEvents(now, dayEnd);
  
  if(events.length > 0){
      return true;
  }else{
    return false;
 }
}

function areEventsHappeningNow(cal){
  var now = new Date();
  var later = new Date();
  later.setMinutes(now.getMinutes() + 1);
  var events = cal.getEvents(now, later);
  
  if(events.length > 0){
    areEventsHappening = true;}
  else{ 
    return false;
      }
}

function getAllEventsHappeningLater(cal){
  var now = new Date();
  var dayEnd = new Date();
  dayEnd.setHours(23,59,59);
  var events = cal.getEvents(now, dayEnd);

  var str = '';
    
  for (var ii = 0; ii < events.length; ii++) {
    var event=events[ii]; 
    str += event.getTitle() +'\n'+
      formatDate(event.getStartTime()) + ' to '+
      formatDate(event.getEndTime()) +'\n';
    }
  return str;
}

function getEvents(cal){
  var now = new Date();
  //var now = Utilities.formatDate(new Date(), "GMT-3", "MM/dd/yyyy HH:mm");
  var later = new Date();
  later.setMinutes(now.getMinutes() + 1);

  var events = cal.getEvents(now, later);
  var str = '';
    
  for (var ii = 0; ii < events.length; ii++) {
    var event=events[ii]; 
    str += event.getTitle() +'\n'+
      formatDate(event.getStartTime()) + ' to '+
      formatDate(event.getEndTime()) +'\n';
    }
  return str;
}

function formatDate(date){
  var formattedDate = Utilities.formatDate(date, "GMT-3", "HH:mm");
  return formattedDate;
}

 /* 
 Firebase
 */

function connectFirebase() {
        return base;
}

function readMeetingStatus() {
        var firebaseUrl = "https://************.firebaseio.com";
        var secret = "*************";
        var base = FirebaseApp.getDatabaseByUrl(firebaseUrl, secret);
        var contact = base.getData("office/meeting");
        Logger.log(contact);
        return contact;
}

function setMeetingStatus(status) {
        var firebaseUrl = "https://**********.firebaseio.com";
        var secret = "***************";
        var base = FirebaseApp.getDatabaseByUrl(firebaseUrl, secret);
        base.setData("office/control/action", "f");
        base.setData("office/meeting", status);
}



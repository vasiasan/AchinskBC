import webapp2
from google.appengine.ext import ndb

from datetime import datetime, timedelta
import json
#import cgi
#import urllib

class temperatureData(ndb.Model):
    """Models an individual Guestbook entry with author, content, and date."""
    time = ndb.DateTimeProperty(auto_now_add=True)
    t1 = ndb.FloatProperty(indexed=False)
    t2 = ndb.FloatProperty(indexed=False)
    t3 = ndb.FloatProperty(indexed=False)
    t4 = ndb.FloatProperty(indexed=False)

def floatConvert( s ):
  if s == "-127.0":
    return None
  try:
    return float(s)
  except ValueError:
    return None

class putData(webapp2.RequestHandler):

    def get(self):
      if self.request.get('key') == "YOURPASSOWRD":
        thisTemp = temperatureData(
                      t1 = floatConvert( self.request.get('t1') ),
                      t2 = floatConvert( self.request.get('t2') ),
                      t3 = floatConvert( self.request.get('t3') ),
                      t4 = floatConvert( self.request.get('t4') ) )
        thisTemp.put()
        self.response.out.write("OK")
      else:
        self.response.set_status(401)

class getData(webapp2.RequestHandler):

    def get(self):
        week = temperatureData.query(temperatureData.time >= ( datetime.now()-timedelta(days=7) ) ).order(temperatureData.time)
        self.response.headers['Content-Type'] = 'application/json'
        weekToJson = []
        for thisVal in week:
            weekToJson.append({'time': thisVal.time.isoformat(), 't1': thisVal.t1, 't2': thisVal.t2, 't3': thisVal.t3, 't4': thisVal.t4})
        self.response.out.write(json.dumps(weekToJson))



application = webapp2.WSGIApplication([
    ('/putData', putData),
    ('/getData', getData),
], debug=True)
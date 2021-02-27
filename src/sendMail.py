# importing the required library. 
import smtplib 
import sys


mailToSend = sys.argv[1]
finalMsg = sys.argv[2]

productMail = "recycleservice1@gmail.com"
productPassword = "nv79456123"

# creates SMTP session 
email = smtplib.SMTP('smtp.gmail.com', 587) 
  
# TLS for security 
email.starttls() 
  
# authentication
# compiler gives an error for wrong credential. 
email.login(productMail, productPassword ) 
  
# message to be sent 
message = """From: %s\nTo: %s\nSubject: %s\n\n%s""" % (mailToSend, ", ".join(mailToSend), "total sum", finalMsg)


# sending the mail 
email.sendmail(productMail, mailToSend, message) # from, to, msg
  
# terminating the session 
email.quit()

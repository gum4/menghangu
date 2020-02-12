rm (list=ls())
library(wooldridge)
library(lmtest)
#install.packages("vars")
library(vars)
#install.packages("rtsdata")
library(rtsdata)
library(quantmod)
library(gdata)

##################################

F1<-read.xls("比特币期货 CME历史数据-2.xlsx",sheet=1,na.strings=c("NA","#DIV/0!"),stringsAsFactors=TRUE)
length(F1[,2])
S1<-read.xls("BTC-USD2018-2020.xlsx",sheet=1,na.strings=c("NA","#DIV/0!"),stringsAsFactors=TRUE)
length(S1[,5])
length(F1[,2])
F1
S1


l<-"-"
z<-"0"
F1[,1]
F1[,1]<-as.character(F1[,1])

Date_F<-matrix(0,nrow=length(F1[,2]),ncol=1)
for (i in 1:length(Date_F)){
  s1<-substr(F1[i,1],1,4)
  if (nchar(F1[i,1])==9){
    s2<-substr(F1[i,1],6,6)
    s3<-substr(F1[i,1],8,8)
    s2<-paste0(z,s2)
    s3<-paste0(z,s3)
    Date_F[i]<-paste0(s1,l,s2,l,s3)
  }
  else if (nchar(F1[i,1])==10){
    if (substr(F1[,1],7,7)=="月"){
      s2<-substr(F1[i,1],6,6)
      s3<-substr(F1[i,1],8,9)
      s2<-paste0(z,s2)
      Date_F[i]<-paste0(s1,l,s2,l,s3)
    }
    if (substr(F1[i,1],8,8)=="月"){
      s2<-substr(F1[i,1],6,7)
      s3<-substr(F1[i,1],9,9)
      s3<-paste0(z,s3)
      Date_F[i]<-paste0(s1,l,s2,l,s3)
    }
  }
  else {
    s2<-substr(F1[i,1],6,7)
    s3<-substr(F1[i,1],9,10)
    Date_F[i]<-paste0(s1,l,s2,l,s3)
  }
}
Date_F
S1[,1]<-as.character(S1[,1])
con <- S1[,1] %in% Date_F

line1<-matrix(S1[1,6],nrow=1,ncol=1)

for (i in 2:length(S1[,6])){
  if (S1[i,1] %in% Date_F){
    line1<-append(line1,S1[i,6])
  }
}

line1<-rev(line1)
Res<-as.data.frame(matrix(NA,nrow=length(Date_F),ncol=3))
names(Res)<-c("Date","Bitcoin Price","Bitcoin Futures Price")
Res$Date<-Date_F
Res$`Bitcoin Price`<-line1
Res$`Bitcoin Futures Price`<-F1$收盘
Res[,3]<-as.character(Res[,3])
for (i in 1:length(Date_F)){
  if (nchar(Res[i,3])==8){
    Res[i,3]<-paste0(substr(Res[i,3],1,1),substr(Res[i,3],3,8))
  }
  if (nchar(Res[i,3])==9){
    Res[i,3]<-paste0(substr(Res[i,3],1,2),substr(Res[i,3],4,9))
  }
}

Res$`Bitcoin Price`<-as.numeric(Res$`Bitcoin Price`)
Res$`Bitcoin Futures Price`<-as.numeric(Res$`Bitcoin Futures Price`)

Res
##################################################


##set p=2 , at most lag 2 terms
Res

dif_BTF<-diff(log(Res$`Bitcoin Futures Price`))
dif_BTC<-diff(log(Res$`Bitcoin Price`))

dif<-cbind(dif_BTF,dif_BTC)
dif
# determine lag terms
VARselect(dif,lag.max=20,type="const")
##set p=4 , at most lag 4 terms
lag_analysis<-VAR(dif,type="const",lag.max = 4,ic="AIC")

lag_analysis

BT<-matrix(NA,nrow=length(dif_BTF)-4,ncol=2)

for (i in 1:length(BT)/2){
   BT[i,1]<-dif_BTF[i]-(-0.45*dif_BTF[i+1]+0.46*dif_BTC[i+1]-0.68*dif_BTF[i+2]+0.79*dif_BTC[i+2]-
                          0.39*dif_BTF[i+3]+0.39*dif_BTC[i+3]-0.24*dif_BTF[i+4]+0.22*dif_BTC[i+4])
   BT[i,2]<-dif_BTF[i]-(0.46*dif_BTF[i+1]-0.44*dif_BTC[i+1]-0.003*dif_BTF[i+2]+0.1*dif_BTC[i+2]+
                          0.06*dif_BTF[i+3]-0.06*dif_BTC[i+3]-0.02*dif_BTF[i+4]-0.0006*dif_BTC[i+4])
}
BT



############################
#predict BTF

#DELTA(ft_hat)=2.919e-03+6.248e-05*(f^t-1-s^t-1)+(-0.45*DELTA(ft-1)+0.46*DELTA(st-1)-0.68*DELTA(ft-2)+0.79*DELTA(st-2)-
#-0.39*DELTA(ft-3)+0.39*DELTA(st-3)-0.24*DELTA(ft-4)+0.22*DELTA(st-4))


#DELTA(ft-i) come from dif_BTF
#DELTA(st-i) come from dif_BTC


Z<-log(Res$`Bitcoin Futures Price`)- log(Res$`Bitcoin Price`)
##BTF stores the difference between lagged terms
Z<-Z[2:(length(Z)-4)]
BT<-cbind(BT,Z)
Result<-as.data.frame(BT)
names(Result)<-c("dif_BTF","dif_BTC","Z")
Result
fit.BTF<-lm(Result$dif_BTF~Result$Z,data=Result)
summary(fit.BTF)
fit.BTC<-lm(Result$dif_BTC~Result$Z,data=Result)
summary(fit.BTC)


################################################
alpha1<-0.489583
alpha2<-1.224821

alpha<-matrix(NA,nrow=2,ncol=1)
alpha[1]<-alpha1
alpha[2]<-alpha2
beta<-matrix(NA,nrow=2,ncol=1)
beta[1]<-1
beta[2]<--1
alpha_hat<-matrix(NA,nrow=2,ncol=1)
alpha_hat[1]<-2
alpha_hat[2]<--alpha_hat[1]*alpha1*1.0/alpha2
beta_hat<-matrix(NA,nrow=2,ncol=1)
beta_hat[1]<-1
beta_hat[2]<-1
alpha_hat

A1<- beta_hat %*% (1.0/(t(alpha_hat) %*% beta_hat)) 
A2<- alpha %*% (1.0/(t(beta) %*% alpha)) 
A1
A2
Res
#ft=0.8329419*(2*ft-0.799436*st)-0.6658837*(ft-st)+0.003984
#st=0.8329419*(2*ft-0.799436*st)-1.6658837*(ft-st)+0.008874

#ft=0.88st-5e-4





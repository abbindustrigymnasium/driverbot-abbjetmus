# node image
FROM node:alpine

#RUN git clone https://github.com/mcollina/mosca.git
WORKDIR /

# install all needed packages
#RUN npm install 
RUN npm install aedes --save && npm install websocket-stream --save

# add the files to load
ADD ./broker.js .

# expose port
EXPOSE 1883
EXPOSE 80

# execute app.js
ENTRYPOINT ["node", "broker.js"]
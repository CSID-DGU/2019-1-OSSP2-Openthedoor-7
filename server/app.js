var createError = require('http-errors');
var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');
const request = require('async-request');
var net = require('net');

var indexRouter = require('./routes/index');
var usersRouter = require('./routes/users');

var app = express();

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'jade');

app.use(logger('dev'));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.use('/', indexRouter);
app.use('/users', usersRouter);

// catch 404 and forward to error handler
app.use(function(req, res, next) {
  next(createError(404));
});

// error handler
app.use(function(err, req, res, next) {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get('env') === 'development' ? err : {};

  // render the error page
  res.status(err.status || 500);
  res.render('error');
});


var server = net.createServer(function(client){
	console.log('client connected');

	client.on('data', async function(data){
		var str = data.toString();
		console.log(str);
		var arr = str.split('|',1);
		console.log('what : '+arr[0]);
		
		if(arr[0] == '1'){
			//---------------------------------- 회원가입하기 
			arr = str.split('|',3);

			let option = {
				method:'POST',
				data: {
					user_id : arr[1],
					user_pw : arr[2]
				}
			};
			let result = await request('http://13.124.167.29:3000/signup',option);

			let data = JSON.parse(result.body).data;
			console.log('data : ' + data);
			console.log('result body object : ' + JSON.parse(result.body).user_idx.toString());
			if(!result){
				console.log("No data");
			} else {
				client.write(JSON.parse(result.body).user_idx.toString(),function(err){
					client.end();
				});

				console.log('data : ' + data);
			}
		}
		else if(arr[0] == '2'){
			//--------------------------------- 로그인하기 
			arr = str.split('|',3);

			let option = {
				method : 'POST',
				data: {
					user_id : arr[1],
					user_pw : arr[2]
				}
			};
			let result = await request('http://13.124.167.29:3000/signin',option);
			console.log('result body object : ' + JSON.parse(result.body).user_idx);
			let data = JSON.parse(result.body).data;
			if(!result){
				console.log("No data");
			} else {
				client.write(JSON.parse(result.body).user_idx.toString(),function(err){
					client.end();
				});

				console.log('data : ' + data);
			}
		}
		else if(arr[0] == '3'){
			//-------------------------------- user_idx로 item 정보 불러오기 (GET)
			arr = str.split('|',2);
			console.log('here1');
			console.log('arr[1]:'+ arr[1]);

			let option = {
				method : 'GET',
			}
			let url = 'http://13.124.167.29:3000/item/' + parseInt(arr[1]);
			console.log(url);
			//let result = await request('http://13.209.29.192:3000/item/'+ arr[1].toString());
			let result = await request(url,option);
			let data = JSON.parse(result.body).data;

			//console.log(JSON.parse(result.body).item[0].item);
			//console.log(typeof JSON.parse(result.body).item[0].item);

			if(!result){
				console.log("No data");
			} else {
				client.write(JSON.parse(result.body).item[0].item.toString(),function(err){
					client.end();
				});
				console.log('data : ' + data);
			}

		}
		else if(arr[0] == '4'){
			//------------------------------- user_idx로 item 정보 update 하기 (PUT)
			arr = str.split('|',3);
			let option = {
				method : 'PUT',
				data: {
					item : arr[1]
				}
			}
			let url = 'http://13.124.167.29:3000/item/' + parseInt(arr[2]);
			let result = await request(url,option);
			let data = JSON.parse(result.body).data;

			if(!result){
				console.log("No data");
			} else {
				client.end();
				/*
				client.write(JSON.stringify(JSON.parse(result.body).item),function(err){
					client.end();
				});*/
				console.log('data : ' + data);
			}
		}
		else if(arr[0] == '5'){
			//-------------------------------- user_idx로 score 정보 불러오기 (GET)
			arr = str.split('|',2);
			console.log('here1');
			console.log('arr[1]:'+ arr[1]);

			let option = {
				method : 'GET',
			}
			let url = 'http://13.124.167.29:3000/score/' + parseInt(arr[1]);
			console.log(url);
			//let result = await request('http://13.209.29.192:3000/item/'+ arr[1].toString());
			let result = await request(url,option);
			let data = JSON.parse(result.body).data;

			console.log(JSON.parse(result.body).score[0].score);
			console.log(typeof JSON.parse(result.body).score[0].score);

			if(!result){
				console.log("No data");
			} else {
				client.write(JSON.parse(result.body).score[0].score.toString(), function(err){
					client.end();
				});
				console.log('data : ' + data);
			}

		}
		else if(arr[0] == '6'){
			//------------------------------- user_idx로 score 정보 update 하기 (PUT)
			arr = str.split('|',3);
			let option = {
				method : 'PUT',
				data: {
					score : arr[1]
				}
			}
			let url = 'http://13.124.167.29:3000/score/' + parseInt(arr[2]); //arr[2]는 user_idx 정보 
			let result = await request(url,option);
			let data = JSON.parse(result.body).data;

			if(!result){
				console.log("No data");
			} else {
				/*
				client.write(JSON.parse(result.body).score[0].score.toString(),function(err){
					client.end();
				});*/
				client.end();
				console.log('data : ' + data);
			}
		}
		else if(arr[0] == '7'){
			//------------------------------ user's rank output
			arr = str.split('|',2);
			let option = {
				method : 'GET',
			}
			let scoreUrl = 'http://13.124.167.29:3000/score/' + parseInt(arr[1]);
			let scoreResult = await request(scoreUrl,option);

			if(!scoreResult){
				console.log("No data");
			} else {
				// user_idx의 점수 : JSON.parse(result.body).score[0].score.toString()
				let score = JSON.parse(scoreResult.body).score[0].score.toString();
				let rankUrl = 'http://13.124.167.29:3000/rank/' + parseInt(score);
				let rankResult = await request(rankUrl,option);

				console.log(JSON.parse(rankResult.body).rank);
				//console.log(typeof JSON.parse(rankResult.body).item[0].item);

				if(!rankResult){
					console.log("No data");
				}else {
					client.write(JSON.parse(rankResult.body).rank.toString(),function(err){
						client.end();
					});
				}
			}

		} else if(arr[0] == '8'){
			//---------------------------- first score get
			let option = {
				method: 'GET'
			}
			console.log('HereHere');
			let firstUrl = 'http://13.124.167.29:3000/first';
			let firstResult = await request(firstUrl,option);

			console.log(JSON.parse(firstResult.body).first);

			if(!firstResult){
				console.log('No data');
			} else {
				client.write(JSON.parse(firstResult.body).first[0].score.toString(),function(err){
					client.end();
				});
			}
		}
		client.on('end', function(){
		console.log('client disconnected');
	});
	});

	client.on('error',function(err){
		console.log('Error : ' + err);
	})
	
})


server.listen(3000, function() { 
   console.log('server is listening');
});

module.exports = app;

var express    = require("express");
var login = require('./routes/loginroutes');
var bodyParser = require('body-parser');

var app = express();

// json으로 들어오는 요청을 파싱하기 위해 bodyparser를 미들웨어로 사용
app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());
app.use(function(req, res, next) {
    res.header("Access-Control-Allow-Origin", "*");
    res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
    next();
});

var router = express.Router();

// test route
router.get('/', function(req, res) {
    res.json({ message: 'welcome to our upload module apis' });
});

//route to handle user registrationrouter.post('/register',login.register);
router.post('/login',login.login)
app.use('/api', router);
app.listen(5000);

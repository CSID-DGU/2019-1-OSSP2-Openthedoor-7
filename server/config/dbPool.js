const mysql = require('promise-mysql');
const config = {
    host: 'openthedoor.carwy5e8omya.ap-northeast-2.rds.amazonaws.com',
    port: 3306,
    user: 'openthedoor',
    password: '2019ossp7',
    database: 'openthedoor',
}
module.exports = mysql.createPool(config);

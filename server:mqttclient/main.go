package main

import (
	"database/sql"
	"fmt"
	"os"
	"time"

	mqtt "github.com/eclipse/paho.mqtt.golang"
	_ "github.com/lib/pq"

)

type Postgre struct {
	TopicName  string
	Payload    string
	DBConn     sql.DB
	ClientMQTT mqtt.Client
}

const (

	// Broker
	topicName = "satc/comdados/#"
	brokerUrl = "broker.hivemq.com:1883"

	// Database
	host     = "localhost"
	port     = 5432
	user     = "postgres"
	password = "barbadovagner"
	dbname   = "mqttarduino"
)



func mqttClient() {
	//// Cria canal go
	//
	//// Conecta no broker com os parametros
	clientOptions := mqtt.NewClientOptions().AddBroker(brokerUrl).SetClientID("ComDadosLuizClient")
	client := mqtt.NewClient(clientOptions)
	//
	//// Puxa o token do broker e verifica se alguem erro occoreu
	if token := client.Connect(); token.Wait() && token.Error() != nil {
	}

	// Faz o subscribe a um topico e recebe mensagem
	if token := client.Subscribe(topicName, 0, func(client mqtt.Client, msg mqtt.Message) {

		tpc := msg.Topic()
		data := string(msg.Payload())

		psqlInfo := fmt.Sprintf("host=%s port=%d user=%s password=%s dbname=%s sslmode=disable", host, port, user, password, dbname)
		db, err := sql.Open("postgres", psqlInfo)
		if err != nil {
			fmt.Println(err)
		}
		
		db.SetConnMaxLifetime(5 * time.Second)
		db.SetMaxOpenConns(0)
		db.SetMaxIdleConns(0)
		//

		sqlInsert := `
			INSERT INTO comdados(
			sensor,
			written_time
			)
			VALUES($1, current_timestamp at time zone 'UTC')`



		var sensorNome string
		//var payloadValue string
		if data == "1" {
			sensorNome = "cartão"
		} else if data == "0" {
			sensorNome = "tag"
		}

		_, err = db.Exec(sqlInsert, sensorNome)
		if err != nil {
			fmt.Println(err)
		}
		fmt.Println(tpc)
		fmt.Println(data)
	});

	// Mostra ao usuario que r
	token.Wait() && token.Error() == nil {
		fmt.Print("Subscription to topic: (" + topicName + ") successful!!\n")
	}

}


func main() {
	c := make(chan os.Signal, 2)

	go mqttClient()
	<-c

	os.Exit(0)

}
